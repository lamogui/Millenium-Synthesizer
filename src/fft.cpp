#include "fft.hpp" 
#include <iostream>

FFT::FFT(unsigned int size) :
  _values(NULL),
  _indexTable(NULL),
  _twidleFactor(NULL),
  _size(0),
  _pow2(FFT::getSupPow2(size)+1)
{
  _size=1<<_pow2;
  this->realloc(size);
}

FFT::~FFT() {
  if (_values) {
    free(_values);
    free(_indexTable);
    free(_twidleFactor);
  }
}

unsigned short FFT::getInfPow2(unsigned int v)
{
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  return bit;
}

unsigned short FFT::getSupPow2(unsigned int v)
{
  unsigned int v_mem=v;
  unsigned short bit=0;
  for (unsigned int i=0; i < sizeof(unsigned int) << 3; i++)
  {
    if (v & 1) bit = i; v >>=1; 
  }
  if (((unsigned)1<<bit)==v_mem)
    return bit;
  else
    return bit+1;
}

//calcul et allocation de l'espace requis pour la fft
//size represente le nombre de point complexe que contient le signal
//dans notre exemple : 9
void FFT::realloc(unsigned int size) {
  //on recupere la puissance de 2 superieur a size, un tableau de la taille 2^_pow2
  //nous permettra donc de contenir le signal
  //pratiquement puisque l'on travail avec des complexes, il nous faut doubler la taille
  //car on stocke les parties reels et imaginaires dans le meme tableau
  //dans notre cas, on obtient 5 car
  //la puissance de 2 superieur a 9 est 16(2^4) puis on ajoute 1 pour doubler _size
  _pow2=getSupPow2(size)+1;
  if (_pow2) {
    //calcul de _size
    //_size est la taille du tableau contenant les parties reels et imaginaires des 
    //points du signal
    //_size doit toujours etre une puissance de 2
    //c'est pour cela que l'on calcul _size par rapport a _pow2
    //_size=2*(puissance de 2 superieur au nbre de points du signal)
    //dans notre exemple 16*2=32
    _size = 1 << _pow2;
    _values=(sample*)std::realloc((void*)_values,_size*sizeof(float));
    //le nombre de twidleFator sera toujours de : nbre de points du signal -1
    //les twidleFactor sont aussi des complexes donc il nous faut allouer le double
    //on alloue donc 2*(nbre de points du signal -1)
    //=2*nbre de points du signal -2
    //=_size-2
    //dans notre exemple 30 c'est a dire : [0, 29]
    _twidleFactor=(sample*)std::realloc((void*)_twidleFactor,(_size-2)*sizeof(float));
    //_indexTable contient tous les index inverse
    //ici on se refere aux index du signal remonte a la puissance de 2 superieur
    //dans notre exemple : 9->16 : index de 0 a 15
    _indexTable=(unsigned int*)std::realloc((void*)_indexTable,
                                            (_size>>1)*sizeof(unsigned int));
  }
  //on initialise les _values a 0
  for (unsigned int i=0; i<_size;i++) {
    _values[i]=0;
  }
  
  //calcul des twidleFactor
  //la formule est : exp(-j*2*PI*n/N)
  //si on explose en partie reel et imaginaire
  //Re(W)=cos(2*PI*n/N)
  //Im(W)=sin(-2*PI*n/N)
  //Pour la FFT, N varie en en multiple de 2 en commencant par 2 : 2, 4, 8, ...
  //N s'arrete a la puissance de 2 superieur au nombre de points du signal
  //dans notre exemple : 16
  //n varie de [0 a N/2[
  //pour le stockage, on les stocke dans l'ordre
  //W02, W04, W14, W08, W18, W28, W38
  //pour les retrouver dans le tableau, il suffit donc juste de faire :
  //_twidleFactor[N_sum+n*2] avec N_sum+=N a la fin de chaque tour de boucle avec 
  //initialisation a 0
  //exemples : Re(W20)=_twidleFactor(0+0)
  //exemples : Im(W20)=_twidleFactor(0+0+1)
  //exemples : Re(W41)=_twidleFactor(2+1)
  //exemples : W83=_twidleFactor(2+4+3)
  sample* glisseur=_twidleFactor;
  //unsigned int N_sum=0;
  for (unsigned int N=2; N<=(_size>>1); N<<=1) {
    for (unsigned int n=0; n<N/2; n++) {
    //  std::cout << N_sum+n*2 << std::endl;
      *glisseur++=std::cos(2.f*M_PI*(float)n/(float)N);
      //std::cout << N_sum+n*2+1 << std::endl;
      *glisseur++=std::sin(-2.f*M_PI*(float)n/(float)N);
    }
      //N_sum+=N;
  }

  //generation du tableau d'index a bit renverse
  //les indexes sont de taille en bits de : puissance de 2 superieur au nombre de 
  //point du signal
  //dans notre exemple : 9 -> 16 : les indexes vont de [0,15]
  //Pour calculer les indexes renverse, on fait :
  //&######0 -> 0######&
  //#&####0# -> #0####&#
  //...
  //Pour la premiere de boucle, il faut donc diviser _size par 2 pour retrouver
  //la bonne taille de l'index
  //la deuxieme boucle boucle sur la taille en bit d'un index
  //g_init donne la position du bit 0
  //g_fin donne la position du bit &
  //g_delta donne l'ecart entre ces deux bits
  if (_pow2) {
    for (unsigned int j=0; j<(_size>>1); j++) {
      unsigned int g_init=0, g_fin=_pow2-2, index=0;
      unsigned int i_delta=_pow2-2;
      for (unsigned int i=0; i<(unsigned)(_pow2>>1); i++) {
        unsigned int b1=0,b2=0;
        b1=(j&(1<<g_init))<<i_delta;
        b2=(j&(1<<g_fin))>>i_delta;
        index=index|(b1|b2);
        g_init++;
        g_fin--;
        i_delta-=2;
      }
      //std::cout << "l'index vaut "<<index << std::endl;
      _indexTable[j]=index;
    }
  }
}


void FFT::compute(const Signal &s) {
  for (unsigned int i=0; i<Signal::size; i++) {
    _values[i<<1]=s.samples[i];
  }

  for (unsigned int i=0; i<_size>>1; i++) {
    std::swap(_values[i],_values[_indexTable[i>>1]]);
    std::swap(_values[i+1],_values[_indexTable[i>>1]+1]);
  }

  unsigned int N_sum=0;
  sample c1_reel=0, c1_img=0, c2_reel=0, c2_img=0, W_reel=0, W_img=0;
  sample *glisseur=_values;
  sample *glisseur_fin=glisseur+_size;
  for (unsigned int N=2; N<=(_size>>1); N<<=1) {
    for (unsigned int n=0; n<N/2; n++) {
      W_reel=_twidleFactor[N_sum+n];
      W_img=_twidleFactor[N_sum+n+1];
      c1_reel=glisseur[0];
      c1_img=glisseur[1];
      c2_reel=glisseur[N];
      c2_reel=glisseur[N+1];
      glisseur[0]=c1_reel + c2_reel*W_reel - c2_img*W_img;
      glisseur[1]=c1_img + c2_reel*W_img + c2_img*W_reel;

      glisseur[N]=c1_reel - c2_reel*W_reel + c2_img*W_img;
      glisseur[N+1]=c1_img - c2_reel*W_img - c2_img*W_reel;
      glisseur++;
    }
    N_sum+=N;
  }
  
}
void FFT::compute(const sample* s, unsigned int size) {
  for (unsigned int i=0; i<size; i++) {
    _values[i<<1]=s[i];
  }

  for (unsigned int i=0; i<_size>>1; i++) {
    std::swap(_values[i<<1],_values[_indexTable[i]<<1]);
    std::cout << "je swap "<<(i<<1)<<" avec "<<(_indexTable[i]<<1)<<std::endl;
    std::swap(_values[(i<<1)+1],_values[(_indexTable[i]<<1)+1]);
    std::cout << "je swap "<<((i<<1)+1)<<" avec "<<((_indexTable[i]<<1)+1)<<std::endl;
  }

  unsigned int N_sum=0;
  sample c1_reel=0, c1_img=0, c2_reel=0, c2_img=0, W_reel=0, W_img=0;
  sample *glisseur=_values;
  sample *glisseur_fin=glisseur+_size;
  for (unsigned int N=2; N<=(_size>>1); N<<=1) {
    for (unsigned int n=0; n<N/2; n++) {
      W_reel=_twidleFactor[N_sum+n];
      W_img=_twidleFactor[N_sum+n+1];
      c1_reel=glisseur[0];
      c1_img=glisseur[1];
      c2_reel=glisseur[N];
      c2_reel=glisseur[N+1];
      glisseur[0]=c1_reel + c2_reel*W_reel - c2_img*W_img;
      glisseur[1]=c1_img + c2_reel*W_img + c2_img*W_reel;

      glisseur[N]=c1_reel - c2_reel*W_reel + c2_img*W_img;
      glisseur[N+1]=c1_img - c2_reel*W_img - c2_img*W_reel;
      glisseur++;
    }
    N_sum+=N;
  }
  
}

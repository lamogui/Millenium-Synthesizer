/*
MIDI.C
Utilitaire servant a decrypter le contenu d'un fichier MIDI.

Auteur: Eric HURTEBIS, France, 2004
(caracteres accentues: d‚sol‚: DOS, Mac, Unix, Windows... too much)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* uncomment for English */
/* #define ENG */

/*
Careful! In English: beat=quarter note
In fact, a tempo (in BPM: beat/mn) is in quarter notes/mn
Do not confuse with "beat" which is part of a "measure".
*/

typedef enum {FAIL=-1, FALSE, SUCCESS=0, TRUE} bool;

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

/* changement de tempo */
typedef struct
  {
  ulong t;              /* date de debut (ticks) */
  uchar nb_temps_mes, unit_temps;       /* signature rythmique */
  ulong ms_q;           /* nb de micro-secondes dans une noire */
  } TEMPO;

int min(int a, int b) {
   if (a>b) {
      return b;
   }
   else {
      return a;
   }
}

void decode_midi(char *fic);
bool read_mtrk(ulong len);
bool read_mthd(ulong len);
void read_time(void);
void sum_time(ulong delta_t, ulong ms_q, uchar unit_temps, uchar nb_temps_mes);
void print_time(void);
bool read_event(void);
bool read_meta(uchar code, short len);
bool read_midi(uchar ordre);
void reset_time(void);
TEMPO *get_tempo(ulong t, ulong dt, int *n);
char *note(short n);
char *instrument(int n);
char *format_MIDI(int n);
char *controller(int n, char *s);
char *decode_sysex(uchar *s, int n, bool *is_text);
ulong ReadVarLen(void);
ushort CombineBytes(uchar First, uchar Second);
void fprint_ascii(FILE *out, uchar *s, int n, bool ascii);
long file_length(FILE *f);
void swap_motorola(char *s, size_t n);
void memrev(char *s, size_t n);
void hexdump(FILE *f);
unsigned char gspart(unsigned char p);
char *find_nrpn(void);

#ifdef ENG
#define ERROR   "error"
#define VERIF(LEN,N)    \
      if (LEN != N)\
        {\
        printf(" " ERROR ": " #LEN " %d instead of " #N "\n", LEN);\
        return FAIL;\
        }
#else
#define ERROR   "erreur"
#define VERIF(LEN,N)    \
      if (LEN != N)\
        {\
        printf(" " ERROR ": " #LEN " %d au lieu de " #N "\n", LEN);\
        return FAIL;\
        }
#endif

#define DESAFF()        \
      if (first_time)\
        {\
        first_time=FALSE;\
        memset(&aff, FALSE, sizeof(aff));\
        }

/*------------------------------------------------------------------------- */

FILE *f;                /* fichier MIDI ouvert */
bool errSingle;         /* erreur si format 1 et piste multi-canaux */
short format,           /* format du fichier (0,1,2) */
        n_track, n_track_cur,   /* nb de piste et numero courant */
        reso;           /* resolution (ticks/noire) */
long reso_s;            /* resolution alternative (ticks/s) */
short fps;              /* frames/seconde */
int running_canal;      /* reperage canal courant (pour running status) */
int canal;              /* canal courant */
uchar running_status;   /* reperage ordre MIDI courant */
long flength;           /* taille du fichier */
ulong time_cur;         /* date de l'evenement courant (ticks) */
ulong delta_t;          /* difference p/r precedent, ticks */

/* elements de temps a cumuler et eventuellement afficher */
ulong nb_temps, nb_mes, frac_ticks,
        ms;     /* temps absolu (micro-secondes) */
long pos_event; /* position de l'evenement dans le fichier */
long last_pos;  /* ancienne position dans fichier (pour hexdump) */

struct
  {
  /* indicateurs d'affichage */
  bool mthd, meta, sysex, midi;
  } aff;
struct
  {
  /* autres options */
  bool t_pos, t_ticks, t_smpte, curieux, dump, hexa;
  } opt;
char *fic_cur;  /* nom du fichier courant */


/* table des changements de tempo (pour les autres pistes du type 1) */
#define MAX_TEM 2000
TEMPO ttem[MAX_TEM];
ushort ntem;

/* mode courant (pour d‚terminer instrument si Bank Select) */
enum {
        GM2,    /* General Midi */
        GS,     /* Roland */
        XG}     /* Yamaha */
        mode_cur;

#define MAX_TR  64      /* nombre maximum de pistes */
unsigned char bank[MAX_TR][2];  /* Bank Select: MSB et LSB (pour chaque canal) */

unsigned char nrpn[MAX_TR][2];  /* RPN/NRPN: MSB et LSB (pour chaque canal) */


/* ======== DEBUT PROGRAMME ========== */

int main(int argc, char **argv)
  {
  bool first_time=TRUE;
  if (argc == 1)
    {
err:
    printf(
#ifdef ENG
        "Syntax: MIDI [options] files ...\n"
        "Decodes a MIDI file. Characters * allowed.\n"
        "Options:\n"
        " -h : displays MThd chunk\n"
        " -m : displays meta\n"
        " -s : displays SysEx\n"
        " -M : displays MIDI\n"
        " -a : displays all except MIDI\n"
        "Default: displays all\n"
        "For the time, displays measure.beat.ticks. Also:\n"
        " -t  : displays absolute time in ticks\n"
        " -tt : displays time in SMPTE (hh:mm:ss.fr)\n"
        " -p  : displays file position\n"
        " -px : id., address in hexa.\n"
        " -P  : displays file position and hexadecimal dump\n"
        " -Px : id., address in hexa.\n"
        " -c  : curious (or gossip) mode: displays on stderr when rarity\n"
#else
        "Syntaxe: MIDI [options] fichiers ...\n"
        "Decode un fichier MIDI. Caracteres * autorises.\n"
        "Options:\n"
        " -h : affiche bloc MThd\n"
        " -m : affiche meta\n"
        " -s : affiche SysEx\n"
        " -M : affiche MIDI\n"
        " -a : affiche tout sauf MIDI\n"
        "Defaut: affiche tout\n"
        "Pour le temps, affiche mesure.temps.ticks. En plus:\n"
        " -t  : affiche temps absolu en ticks\n"
        " -tt : affiche temps en SMPTE (hh:mm:ss.fr)\n"
        " -p  : affiche position fichier\n"
        " -px : idem, position en hexadecimal\n"
        " -P  : affiche position fichier et contenu en hexadecimal\n"
        " -Px : idem, position en hexadecimal\n"
        " -c  : mode curieux (ou commere): affiche sur stderr si rarete\n"
#endif
        );
    return 1;
    }
  memset(&aff, TRUE, sizeof(aff));      /* affiche tout par defaut */
  while (--argc)
    {
    argv++;
    if (strcmp(*argv, "-p") == SUCCESS)
      opt.t_pos = TRUE;
    else if (strcmp(*argv, "-px") == SUCCESS)
      opt.t_pos = opt.hexa = TRUE;
    else if (strcmp(*argv, "-P") == SUCCESS)
      opt.dump = opt.t_pos = TRUE;
    else if (strcmp(*argv, "-Px") == SUCCESS
        || strcmp(*argv, "-PX") == SUCCESS)
      opt.dump = opt.t_pos = opt.hexa = TRUE;
    else if (strcmp(*argv, "-t") == SUCCESS)
      opt.t_ticks = TRUE;
    else if (strcmp(*argv, "-tt") == SUCCESS)
      opt.t_smpte = TRUE;
    else if (strcmp(*argv, "-c") == SUCCESS)
      {
      opt.curieux = TRUE;
      DESAFF();
      }
    else if (**argv=='-' && argv[0][2]==0)
      switch(argv[0][1])
        {
        case 'a':
          aff.mthd = aff.meta = aff.sysex = TRUE;
          aff.midi = FALSE;
          break;
        case 'h':
          DESAFF();
          aff.mthd = TRUE;
          break;
        case 'm':
          DESAFF();
          aff.meta = TRUE;
          break;
        case 's':
          DESAFF();
          aff.sysex = TRUE;
          break;
        case 'M':
          DESAFF();
          aff.midi = TRUE;
          break;
        default:
          goto err;
        }
    else if (**argv=='-' || **argv=='+' || **argv=='/')
      goto err;
    else
      decode_midi(*argv);
    }
  return 0;
  }


/* affiche le contenu d'un fichier MIDI */
void decode_midi(char *fic)
  {
  /* identifiant de paquet de donnees (MThd, MTrk...) */
  struct
    {
    char id[4];
    ulong len;
    } h;
  short n;
  bool err=FALSE;

  if (! (f=fopen(fic, "rb")))
    {
    perror(fic);
    return;
    }
  fic_cur = fic;
  flength = file_length(f);
  if (! opt.curieux)
    {
    printf("%s (len=%ld", fic, flength);
    if (opt.hexa)
      printf("=%05lX", flength);
    puts("):");
    }
  n_track_cur = 0;
  errSingle=FALSE;
  last_pos=0;
  reset_time(); /* valeurs par defaut du temps */
  mode_cur = GM2;
  memset(bank, 0, sizeof(bank));
  memset(nrpn, 0, sizeof(nrpn));
  for (;;)
    {
    /* lecture de l'en-tete du paquet suivant */
    n = fread(&h, 1, sizeof(h), f);
    if (n == 0)
      break;
    if (opt.dump)
      hexdump(f);
    if (n != sizeof(h))
      {
      printf("%d "
#ifdef ENG
        "bytes overread"
#else
        "octets superflus lus"
#endif
        ": ", n);
      fprint_ascii(stdout, (uchar *)&h, n, TRUE);
      puts("");
      err=TRUE;
      break;
      }
    swap_motorola((char *)&h.len, sizeof(h.len));       /* conversion entier en IBM-PC */
    if (strnicmp(h.id, "MThd", 4) == 0)
      {
      if (read_mthd(h.len) != SUCCESS)  /* lit l'en-tete "methode" */
        {
        err=TRUE;
        break;
        }
      }
    else if (strnicmp(h.id, "MTrk", 4) == 0)
      {
      if (read_mtrk(h.len) != SUCCESS)  /* lit une piste */
        {
        err=TRUE;
        break;
        }
      }
    else
      {
      /* paquet inconnu, on respecte et on se deplace au suivant */
      long pos, pos0 = ftell(f);
      if (fseek(f, h.len, SEEK_CUR) != SUCCESS)
        {
        perror("fseek");
        err=TRUE;
        break;
        }
      pos = pos0 + h.len;
      if (ftell(f) < 0 || pos > flength)
        {
        printf(
#ifdef ENG
                "Unknown chunk"
#else
                "Paquet inconnu"
#endif
                " \"");
        fprint_ascii(stdout, (uchar *)h.id, 4, TRUE);
        printf("\" "
#ifdef ENG
                "too large: size %lu, position"
#else
                "trop grand: taille %lu, position"
#endif
                " %ld/%ld\n",
                h.len, pos0-sizeof(h), flength);
        err=TRUE;
        break;
        }
      else if (opt.curieux)
        fprintf(stderr,
#ifdef ENG
                "%s: unknown chunk \"%.4s\", "
                "size %lu, position %ld\n",
#else
                "%s: paquet inconnu \"%.4s\", "
                "taille %lu, position %ld\n",
#endif
                fic, h.id, h.len, pos0-sizeof(h));
      }
    }
  fclose(f);
  if (err)
    fprintf(stderr, "%s: "
#ifdef ENG
        "corrupted"
#else
        "errone"
#endif
        "...\n", fic);
  if (n_track_cur != n_track)
    fprintf(stderr, "%s: "
#ifdef ENG
        "not enough tracks: %d on"
#else
        "nombre insuffisant de pistes: %d sur"
#endif
        " %d\n",
                fic, n_track_cur, n_track);
  if (format == 0 && n_track > 1)
    fprintf(stderr, "%s: "
#ifdef ENG
        "more than 1 track"
#else
        "plusieurs pistes"
#endif
        " (format 0) (%d)\n", fic, n_track);
  if (errSingle)
    if (n_track == 1)
      fprintf(stderr, "%s: "
#ifdef ENG
        "in fact a type 0, not 1"
#else
        "ce type 1 est en fait un type 0"
#endif
        ".\n", fic);
    else
      fprintf(stderr, "%s: "
#ifdef ENG
        "more than 1 channel/track encountered (and format 1)"
#else
        "parfois plusieurs canaux par piste (et format 1)"
#endif
        ".\n", fic);
  if (format == 2 && n_track == 1)
    fprintf(stderr, "%s: "
#ifdef ENG
        "this type 2 is in fact 0 (because 1 track)"
#else
        "ce type 2 revient a un type 0 (car 1 piste)"
#endif
        ".\n", fic);
  if (format == 1 && ntem >= MAX_TEM && n_track > 1)
    fprintf(stderr, "%s: "
#ifdef ENG
        "wrong beat for other tracks (nb max.tempos overflowed"
#else
        "temps faux pour autres pistes (depassement nb max.tempo"
#endif
        ": %u)\n", fic, ntem);
  if (opt.curieux && n_track > 16)
    fprintf(stderr, "%s: %d "
#ifdef ENG
        "track"
#else
        "piste"
#endif
        "s (> 16)\n", fic_cur, n_track);
  assert(n_track <= MAX_TR);
  }


/* lit le bloc "methode" */
bool read_mthd(ulong len)
  {
  signed char buf[6];

  if (aff.mthd)
    puts("\nMethod"
#ifndef ENG
        "e"
#endif
        ":");
  VERIF(len,6); /* quoique... a l'avenir ca puisse faire plus... */
  if (fread(buf, 1, sizeof(buf), f) != len)
    {
    puts(
#ifdef ENG
        "MThd: bad block"
#else
        "Bloc MThd: mauvais"
#endif
        );
    return FAIL;
    }
  if (opt.dump)
    hexdump(f);
  swap_motorola(buf, 2);
  swap_motorola(buf+2, 2);
  format = *(short *)buf;
  n_track = *(short *)(buf+2);
  if (aff.mthd)
    {
    printf("  Type = %d (%s)\n", format, format_MIDI(format));
    printf("  "
#ifdef ENG
        "Number of tracks"
#else
        "Nombre de pistes"
#endif
        ": %d\n", n_track);
    }
  if (format < 0 || format > 2 || n_track <= -1)
    {
    printf("---> " ERROR ": "
#ifdef ENG
        "impossible format"
#else
        "format impossible"
#endif
        "\n");
    return FAIL;
    }
  if (aff.mthd)
    printf("  Resolution: ");
  fps=30;       /* defaut */
  if (buf[4]<0)  /* SMPTE */
    {
    /*
    -buf[4]: frames/seconde
    buf[5]: ticks/frame
    */
    fps = -buf[4];  /* Frames/second */
    if (fps != 24 && fps != 25 && fps != 29 && fps != 30)
      {
      printf("%02X %02X: " ERROR "\n",
        (uchar)buf[4], (uchar)buf[5]);
      return FAIL;
      }
    reso_s = (long)fps * (uchar)buf[5];
    if (aff.mthd)
      printf("%ld ticks/second"
#ifdef ENG
        " (and"
#else
        "e (et"
#endif
        " %u Frames/s)\n", reso_s, fps);
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "resolution in SMPTE (ticks/second)"
#else
        "resolution en SMPTE (ticks/seconde)"
#endif
        "\n", fic_cur);
    }
  else
    {
    swap_motorola(buf+4, 2);
    reso = *(short *)(buf+4);
    if (aff.mthd)
      printf("%d ticks/"
#ifdef ENG
        "quarter"
#else
        "noire"
#endif
      "\n", reso);
    reso_s = 0;
    }
  if (aff.mthd)
    puts("");
  return SUCCESS;
  }


/* lit un bloc "piste" */
bool read_mtrk(ulong len)
  {
  long pos, pos0;

  pos0= ftell(f);
  if (! opt.curieux)
    printf(
#ifdef ENG
        "Track"
#else
        "Piste"
#endif
        " %d (len=%ld):\n", n_track_cur, len);
  n_track_cur++;
  time_cur=0;
  running_canal = -1;
  if (format==2)        /* pistes independantes dans le temps */
    reset_time();
  /* on s'assure de suite que le fichier soit assez grand */
  if (len > flength-pos0)
    {
    fprintf(stderr, "%s: "
#ifdef ENG
        "MTrk size error: %ld and not %ld bytes"
#else
        "erreur taille MTrk: %ld et non %ld octets"
#endif
        " (%lX < %lX).\n",
        fic_cur,
        flength-pos0, len,
        flength-pos0, len);
    len = flength-pos0;
    }

  running_status = 0;
  pos = pos0;
  while (pos >= 0 && pos < pos0+len)
    {
    read_time();
    if (read_event() == FAIL)
      return FAIL;
    if (opt.dump)
      hexdump(f);
    pos = ftell(f);
    }
  if (pos != pos0+len)
    {
    /* vraisemblablement erreur len */
    fprintf(stderr,
        "%s: pos=%ld != pos0+len=%ld (pos0=%ld len=%ld flength=%ld).\n",
        fic_cur, pos, pos0+len, pos0, len, flength);
#if(1)
    return FAIL;
#else
    fseek(f, pos0+len, SEEK_SET);
#endif
    }
  return SUCCESS;
  }


/* reinitialise signature rythmique et tempo aux valeurs par defaut */
void reset_time()
  {
  ntem=1;
  ttem[0].t = 0;
  ttem[0].nb_temps_mes = 4;     /* signature: 4/4 */
  ttem[0].unit_temps   = 2;
  ttem[0].ms_q = 500000L;       /* tempo: 120 noires/mn */
  }


/* lit l'instant de l'evenement et calcule le cumul */
void read_time()
  {
  TEMPO *pt;
  int n;
  ulong dt;
  register i;

  pos_event = ftell(f);
  delta_t = ReadVarLen();
  if (time_cur == 0)
    nb_mes = nb_temps = frac_ticks = ms = 0;
  /*
  Il faut intermixer les evenements de tempo qu'il pourrait y avoir entre
  time_cur et time_cur+delta_t pour calculer correctement les cumuls
  (SMPTE et mesure:temps:ticks),
  utile seulement dans le cas du format 1 et autres pistes que la 1ere
  (car le tempo y est stocke pour toutes les pistes).
  Dans les autres cas (format 0 ou 2 ou piste 1), get_tempo ramene le
  dernier tempo (tempo en cours).
  */
  pt = get_tempo(time_cur, delta_t, &n);
  for (i=0; i<n; i++)
    {
    if (i>0 && i<n-1)
      dt = pt[i+1].t - pt[i].t;
    else if (i==0)
      dt = (n>=2 ? pt[1].t - time_cur : delta_t);
    else  /* i==n-1 && n>=2 */
      dt = time_cur+delta_t - pt[i].t;
    /* somme les temps par intervalles (tempo et signature constants) */
    sum_time(dt, pt[i].ms_q, pt[i].unit_temps, pt[i].nb_temps_mes);
    }
  time_cur += delta_t;  /* temps courant (ticks) */
  }


/* calcule le cumul du temps (ms, nb_temps, nb_mes, frac_ticks) */
void sum_time(ulong delta_t, ulong ms_q, uchar unit_temps, uchar nb_temps_mes)
  {
  ushort reso_t;
  double delta_ms;      /* et non pas "long": le produit de 2 "long" peut depasser un "long" */

  if (reso_s)
    reso = (short)((reso_s * (double)ms_q) / 1000000.);
  /*
  reso est en ticks/noire
  nous, on veut une reso en ticks/temps
  */
  reso_t = reso;
  if (unit_temps < 2)
    reso_t <<= (2 - unit_temps);
  else if (unit_temps > 2)
    reso_t >>= (unit_temps - 2);

  frac_ticks += delta_t;
  nb_temps += frac_ticks / reso_t;
  frac_ticks %= reso_t;
  nb_mes += nb_temps /  nb_temps_mes;
  nb_temps %=  nb_temps_mes;

  /* difference de temps en micro-secondes */
  delta_ms =
        (reso_s ?
        (delta_t * 1000000.) / reso_s :         /* reso_s: ticks/s */
        (delta_t *  (double)ms_q) / reso);      /* 1 noire = "ms_q" 1e-6 s = "reso" ticks */
  ms += (ulong)delta_ms;
  }


/* affiche l'instant de l'evenement */
void print_time()
  {
  if (opt.t_pos)
    if (opt.hexa)
      printf("%05lX: ", pos_event);
    else
      printf("%6ld: ", pos_event);
  if (opt.dump)
    printf("(%+5ld)", delta_t);
  if (opt.t_ticks)
    printf("%6ld:", time_cur);
  /* mesure.temps.ticks */
  printf("%3ld.%03ld.%03ld: ", nb_mes+1, nb_temps+1, frac_ticks);

  if (opt.t_smpte)
    {
    ulong t_sec, t_min, frac_ms;
    ushort t_h, frames;
    t_sec = ms / 1000000L;
    frac_ms = ms - t_sec * 1000000L;
    frames = (ushort)((frac_ms * fps) / 1000000L);
    t_min = t_sec/60;
    t_sec %= 60;
    t_h = (ushort)(t_min / 60);
    t_min %= 60;
    /* heure:minute:secondes.frames */
    printf("%d:%02d:%02d.%02d: ",
        t_h, (ushort)t_min, (ushort)t_sec, frames);
    }
  }


/* lit l'evenement */
bool read_event()
  {
  uchar status, code;
  bool ok=SUCCESS;
  ulong len;
  uchar sysex[128];     /* quelque GS a 137 */

  status = getc(f);
  if (status >= 0x80)   /* ordre MIDI ou meta ou SysEx */
    {
    if (status == 0xFF && aff.meta
    || (status==0xF0 || status==0xF7) && aff.sysex
    || status!=0xFF && status!=0xF0 && status!=0xF7 && aff.midi)
      {
      print_time();
      printf("%02X ", status);
      }
    }
  else  /* donnees MIDI avec ordre MIDI sous-entendu (running status) */
    {
    if (aff.midi)
      {
      print_time();
      printf("   ");
      }
    ungetc(status, f);
    if (running_status)
      status = running_status;
    else
      {
      printf("Bad status (%02X)\n", status);
      return FAIL;
      }
    }
  /* selon le code "status" */
  if (status == 0xFF)   /* meta event (non MIDI) */
    {
    /* 2 prochains octets: sous-code et longueur variable de la suite */
    code = getc(f);
    len = ReadVarLen();
    if (aff.meta)
      printf("%02X ", code);
    if (read_meta(code, len) == FAIL)
      return FAIL;
    }
  else if (status == 0xF0 || status == 0xF7)    /* SysEx */
    {
    ushort i, len_sysex;
    char *p;
    bool is_text=FALSE;

    len = len_sysex = ReadVarLen();
    if (aff.sysex)
      printf("SysEx:");

    if (len > sizeof(sysex))
      {
      if (opt.curieux)
        fprintf(stderr, "%s: "
#ifdef ENG
                "big"
#else
                "gros"
#endif
                " SysEx (%d)\n", fic_cur, len);
      len_sysex = sizeof(sysex);
      }
    fread(sysex, len_sysex, 1, f);
    if (len > len_sysex)
      fseek(f, (long)len - len_sysex, SEEK_CUR);
    for (i=0; i<len_sysex-1; i++)
      if (sysex[i] == 0xF7)
        {
        len_sysex = i+1;
        if (aff.sysex)
          printf(" (" ERROR " len:%ld)", len_sysex);
        if (opt.curieux)
          fprintf(stderr, "%s: "
#ifdef ENG
                "wrong SysEx length"
#else
                "longueur SysEx erronee"
#endif
                "\n", fic_cur);
        break;
        }
    p= decode_sysex(sysex, len_sysex, &is_text);
    if (p && strstr(p, "Reset"))
      memset(bank, 0, sizeof(bank));
    if (aff.sysex)
      {
      if (p)
        printf(" (%s)", p);
      if (is_text)
        putchar(' ');
      fprint_ascii(stdout, sysex, len_sysex, is_text);
      }
    if (aff.sysex)
      printf("%s\n", len>len_sysex ? "...":"");
    }
  else          /* message MIDI */
    {
    uchar ordre_midi;

    running_status = status;
    canal = (status & 0x0F);
    ordre_midi = status - canal;
    if (running_canal == -1)    /* debut de piste */
      running_canal = canal;
    if (aff.midi)
      printf("[%02d] %02X: ", canal, ordre_midi);
    if (read_midi(ordre_midi) == FAIL)
      return FAIL;
    if (format==1 && canal != running_canal)
      errSingle=TRUE;
    }
  return ok;
  }


/* lit un meta-evenement */
bool read_meta(uchar code, short len_data)
  {
  uchar buf[256];
  short len = len_data;
  TEMPO *pt;

  if (len_data > sizeof(buf))
    {
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "big"
#else
        "gros"
#endif
        " meta (%d)\n", fic_cur, len_data);
    len = sizeof(buf);
    }
  fread(buf, len, 1, f);
  if (len_data > len)
    fseek(f, (long)len_data - len, SEEK_CUR);

  if (code >= 0x01 && code <= 0x09)
    {
    char *tab[]=
        {
#ifdef ENG
        "Text",
        "Copyright",
        "Sequence name",
        "Instrument",
        "Lyrics",
        "Mark",
        "Queue point",          /* rare */
        "Instrument name",      /* rare */
        "MIDI port name"                /* 0x09 remplace 0x21 */
#else
        "Texte",
        "Copyright",
        "Nom sequence",
        "Instrument",
        "Paroles",
        "Marqueur",
        "Indication",           /* rare */
        "Nom d'instrument",     /* rare */
        "Nom port MIDI"         /* 0x09 remplace 0x21 */
#endif
        };
    if (aff.meta)
      {
      /*if (code==3 && format==1)
        strcpy(tab[2],
#ifdef ENG
                "Track name"
#else
                "Nom piste"
#endif
                );*/
      printf("%s \"", tab[code-1]);
      fprint_ascii(stdout, buf, len, TRUE);
      puts("\"");
      }
    if (opt.curieux && (code==7 || code==8))
      fprintf(stderr, "%s: rare meta (%s)\n", tab[code-1]);
    return SUCCESS;
    }

  switch(code)
    {
    case 0x20:
      VERIF(len,1);
      if (aff.meta)
        printf(
#ifdef ENG
                "Channel number"
#else
                "Numero canal"
#endif
                " (obsolete): %d\n", *buf);
      break;
    case 0x21:
      VERIF(len,1);
      if (aff.meta)
        printf(
#ifdef ENG
                "MIDI port number"
#else
                "Numero port MIDI"
#endif
                " (obsolete): %d\n", *buf);
      break;
    case 0x2F:
      VERIF(len,0);
      if (aff.meta)
        printf(
#ifdef ENG
                "End of track"
#else
                "Fin de piste"
#endif
                "\n\n");
      break;
    case 0x51:
      /* tempo */
      VERIF(len,3);
      if (format == 1 && n_track_cur > 1)
        {
        fprintf(stderr, "%s: "
#ifdef ENG
                "tempo on track %d: ignored"
#else
                "tempo sur piste %d: ignore"
#endif
                "\n", fic_cur, n_track_cur);
        pt=0;
        }
      else
        {
        ntem++;
        pt = ttem + min(ntem, MAX_TEM) - 1;
        pt->nb_temps_mes = (pt-1)->nb_temps_mes;
        pt->unit_temps = (pt-1)->unit_temps;
        pt->t = time_cur;
        }
      {
      char ms[4]="";
      int tempo;
      long ms_q;
      if (*(short *)"01" > 1)   /* si cette machine n'a pas l'ordre Motorola */
        {
        ms[2]=buf[0];   /* ordre octets: IBM PC */
        ms[1]=buf[1];
        ms[0]=buf[2];
        }
      else
        {
        ms[0]=buf[0];   /* ordre octets: Motorola */
        ms[1]=buf[1];
        ms[2]=buf[2];
        }
      ms_q = *(long *)ms;
      tempo = (int)(60000000L / ms_q);
      if (aff.meta)
        printf("Tempo: %d "
#ifdef ENG
                "BPM (1 quarter"
#else
                "noires/mn (1 noire"
#endif
                " = %ld ms)\n", tempo, ms_q);
      if (pt)
        pt->ms_q = ms_q;
      }
      break;
    case 0x54:
      VERIF(len,5);
      if (aff.meta)
        {
        uchar hh = (buf[0] & 0x1F);
        uchar tt = (buf[0] >> 5);
        int typ[]={24,25,29,30};
        printf(
#ifdef ENG
                "Track begins at"
#else
                "Debut piste a"
#endif
                " %02dh %02dmn %02ds %02d.%02dfr (%d fr/s)\n",
                hh,buf[1],buf[2],buf[3],buf[4], typ[tt]);
        }
      if (opt.curieux)
        fprintf(stderr, "%s: "
#ifdef ENG
                "track starting time is mentioned"
#else
                "heure de debut de piste mentionnee"
#endif
                " (meta)\n", fic_cur);
      break;
    case 0x58:
      /* time signature */
      VERIF(len,4);
      if (format == 1 && n_track_cur > 1)
        fprintf(stderr, "%s: signature "
#ifdef ENG
                "on track %d: ignored"
#else
                "sur piste %d: ignoree"
#endif
                "\n", fic_cur, n_track_cur);
      else
        {
        ntem++;
        pt = ttem + min(ntem,MAX_TEM) - 1;
        pt->nb_temps_mes = buf[0];  /* nb de temps dans une mesure */
        pt->unit_temps = buf[1];          /* valeur d'un temps (2=noire, 3=croche...) */
        pt->t = time_cur;
        pt->ms_q = (pt-1)->ms_q;
        }
      if (aff.meta)
        {
        printf("signature=%d/%d", buf[0], 1 << buf[1]);
        printf("  metronome=%d MIDI clocks", buf[2]);
        if (buf[3] != 8)
          printf("  "
#ifdef ENG
                "quarter=%d 32nds"
#else
                "noire=%d 32emes"
#endif
                , buf[3]);
        puts("");
        }
      if (opt.curieux && buf[3] != 8)
        fprintf(stderr, "%s: 24 MIDI clocks <> "
#ifdef ENG
                "quarter: %d 32nds (instead of"
#else
                "noire: %d 32emes (au lieu de"
#endif
                " 8)\n", fic_cur, buf[3]);
      break;
    case 0x59:
      {
      /* key signature */
      int c=(signed char)buf[0];
      int m=(signed char)buf[1];
      char *cle[]={
           "Solb",      /* bbbbbb */
           "Do#",       /* bbbbb */
           "Sol#",      /* bbbb */
           "Mib",       /* bbb */
           "Sib",       /* bb */
           "Fa",        /* b */
           "Do",
           "Sol",       /* # */
           "Re",        /* ## */
           "La",        /* ### */
           "Mi",        /* #### */
           "Si",        /* ##### */
           "Fa#"        /* ###### */
           };
      VERIF(len,2);
      if (! ((m==0 || m==1) && c >= -6 && c <= 6))
        printf(
#ifdef ENG
                "key"
#else
                "cle"
#endif
                ": %d %d (" ERROR ")\n", c,m);
      else if (aff.meta)
        printf(
#ifdef ENG
                "key"
#else
                "cle"
#endif
                ": %s %s\n", cle[c+6], m?"min":"maj");
      }
      break;
    case 0x7F:
      if (aff.meta)
        {
        printf("Proprietary data");
        if (len>2 && buf[0]==0x05 && buf[1]==0x0F)
          printf(": Anvil Studio");     /* version, Synth, TrackFlags, AudioVol SynthVol AudioMixVol */
        else if (len==3 && buf[0]==0x00 && buf[1]==0x00)
          printf(": Cakewalk"); /* ? */
        printf(" (len=%d data=", len_data);
        fprint_ascii(stdout, buf, min(len,15), TRUE);
        printf("%s)\n", len>15 ? "..." : "");
        }
      if (opt.curieux)
        {
        fprintf(stderr, "%s: Proprietary data (", fic_cur);
        fprint_ascii(stderr, buf, min(len,10), TRUE);
        fprintf(stderr, "%s)\n", len>10 ? "..." : "");
        }
      break;
    default:
      printf(
#ifdef ENG
        "unknown meta"
#else
        "meta inconnu"
#endif
        " (code=%02X,len=%d):", code, len_data);
      fprint_ascii(stdout, buf, min(len,15), FALSE);
      printf("%s\n", len>15?"...":"");
      if (opt.curieux)
        fprintf(stderr, "%s: "
#ifdef ENG
                "unknown meta code"
#else
                "code meta inconnu"
#endif
                " (%02X)\n", fic_cur, code);
      break;
    }
  return SUCCESS;
  }


/* lit un message MIDI */
bool read_midi(uchar ordre)
  {
  uchar buf[2];
  char s[80], *p;
  bool ok=SUCCESS;
  ushort pw;

  switch(ordre)
    {
    case 0x80:  /* note off */
      fread(buf, 1,2,f);
      p = note(*buf);
      if (aff.midi)
        {
        printf("Note off: %3s", p);
        if (buf[1])
          printf(" vel=%d", buf[1]);
        puts("");
        }
      break;
    case 0x90:  /* note on */
      fread(buf, 1,2,f);
      p = note(*buf);
      if (aff.midi)
        {
        printf("Note %-3s: %3s", buf[1] ? "on" : "-", p);
        if (buf[1])
          printf(" vel=%d", buf[1]);
        puts("");
        }
      break;
    case 0xA0:  /* aftertouch */
      fread(buf, 1,2,f);
      p = note(*buf);
      if (aff.midi)
        printf("Aftertouch: %s "
#ifdef ENG
                "pressure"
#else
                "pression"
#endif
                " %d\n", p, buf[1]);
      break;
    case 0xB0:  /* controller */
      fread(buf, 1,2,f);
      controller(*buf,s);
      if (aff.midi)
        printf("CC%d%s "
#ifdef ENG
                "to"
#else
                "a"
#endif
                " %d\n", *buf, s, buf[1]);
      /* Bank Select */
      if (*buf == 0)
        bank[canal][0] = buf[1];        /* MSB */
      else if (*buf == 32)
        bank[canal][1] = buf[1];        /* LSB */
      /* RPN/NRPN */
      if (*buf == 101 || *buf == 99)
        nrpn[canal][0] = buf[1];        /* MSB */
      else if (*buf == 100 || *buf == 98)
        nrpn[canal][1] = buf[1];        /* LSB */
      if (strstr(s, "Reset all"))
        {
        /* reset all controllers */
        memset(bank, 0, sizeof(bank));
        memset(nrpn, 0, sizeof(nrpn));
        }
      break;
    case 0xC0:  /* program change */
      fread(buf, 1,1,f);
      p = instrument(*buf);
      if (aff.midi)
        {
        printf("Instrument %d (%s)", *buf, p);
        /* en toute rigueur, il faudrait table des instruments GS, XG... */
        if (mode_cur != GM2 && (bank[canal][0] || bank[canal][1]))
          {
          if (mode_cur==GS)
            printf(" GS %d", bank[canal][0]);
          else if (mode_cur==XG)
            printf(" XG %d %d", bank[canal][0], bank[canal][1]);
          else
            printf(" ?? %d %d", bank[canal][0], bank[canal][1]);
          }
        puts("");
        }
      break;
    case 0xD0:  /* channel pressure */
      fread(buf, 1,1,f);
      if (aff.midi)
        printf("Channel pressure: %d\n", *buf);
      break;
    case 0xE0:  /* pitch wheel */
      fread(buf, 1,2,f);
      pw = CombineBytes(buf[0], buf[1]);
      if (aff.midi)
        printf("Pitch wheel %04X (%+d)\n", pw, pw-0x2000);
      break;
    default:
      /* mauvais signe: on doit sans doute lire d'autres octets */
      printf(
#ifdef ENG
        "unknwon MIDI code"
#else
        "code MIDI inconnu"
#endif
        " (%02X)\n", ordre);
      ok=FAIL;
      break;
    }
  return ok;
  }


/* lit un entier stocke en longueur variable */
ulong ReadVarLen()
  {
  register ulong value;
  register uchar c;

  if ( (value = getc(f)) & 0x80 )
    {
    value &= 0x7F;
    do
      {
      value = (value << 7) + ((c = getc(f)) & 0x7F);
      } while (c & 0x80);
    }
  return(value);
  }


/* s, pointeur sur entier ordre Motorola (poids fort en 1er), a convertir */
void swap_motorola(char *s, size_t n)
  {
  if (*(short *)"01" > 1)       /* si cette machine n'a pas l'ordre Motorola */
    memrev(s,n);                /* on inverse poids fort et faible */
  }


/* inverse l'ordre des n octets de s[] */
void memrev(char *s, size_t n)
  {
  char c;
  size_t i;

  for (i=0; i<n/2; i++)
    {
    c = s[i];
    s[i] = s[n-1-i];
    s[n-1-i] = c;
    }
  }


/* affiche en clair une chaine de caracteres (ou leur code) */
void fprint_ascii(FILE *out, uchar *s, int n, bool ascii)
  {
  register i;
  for (i=0; i<n; i++)
    if (! ascii)
      fprintf(out, " %02X", s[i]);
    else if (isprint(s[i]))
      putc(s[i], out);
    else
      fprintf(out, "\\x%02X", s[i]);
  }


/* renvoie le nom de la note MIDI n */
char *note(short n)
  {
  static char s[5];
  short octave, num;
  char *tab[]={"C ","C#","D ","D#","E ","F ","F#","G ","G#","A ","A#","B "};

  if (n<0 || n>127)
    {
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "wrong"
#else
        "fausse"
#endif
        " note (%d)\n", fic_cur, n);
    strcpy(s, "???");
    }
  else
    {
    octave = n/12;
    num = n%12;
    sprintf(s, "%s%d", tab[num], octave);
    }
  return s;
  }


/* retourne un entier code sur 14 bits */
ushort CombineBytes(uchar First, uchar Second)
  {
  ushort _14bit;
  _14bit = (ushort)Second;
  _14bit <<= 7;
  _14bit |= (ushort)First;
  return _14bit;
  }


/* retourne le nom du format MIDI */
char *format_MIDI(int n)
  {
  char *tab[]={
#ifdef ENG
    "single multi-channels track",
    "mono-channel tracks",
    "independant multi-channels tracks"};
#else
    "piste multi-canaux unique",
    "plusieurs pistes mono-canal",
    "pistes multi-canaux successives"};
#endif
  if (n<0 || n>2)
    return "???";
  return tab[n];
  }


/* retourne le nom de l'intrument General MIDI n */
char *instrument(int n)
  {
  char *tab[]={
#if(1)
/* #ifdef ENG */
    "Acoustic Grand",
    "Bright Acoustic",
    "Electric Grand",
    "Honky-Tonk",
    "Electric Piano 1",
    "Electric Piano 2",
    "Harpsichord",
    "Clavinet",
    "Celesta",
    "Glockenspiel",
    "Music Box",
    "Vibraphone",
    "Marimba",
    "Xylophone",
    "Tubular Bells",
    "Dulcimer",
    "Drawbar Organ",
    "Percussive Organ",
    "Rock Organ",
    "Church Organ",
    "Reed Organ",
    "Accoridan",
    "Harmonica",
    "Tango Accordian",
    "Nylon String Guitar",
    "Steel String Guitar",
    "Electric Jazz Guitar",
    "Electric Clean Guitar",
    "Electric Muted Guitar",
    "Overdriven Guitar",
    "Distortion Guitar",
    "Guitar Harmonics",
    "Acoustic Bass",
    "Electric Bass(finger)",
    "Electric Bass(pick)",
    "Fretless Bass",
    "Slap Bass 1",
    "Slap Bass 2",
    "Synth Bass 1",
    "Synth Bass 2",
    "Violin",
    "Viola",
    "Cello",
    "Contrabass",
    "Tremolo Strings",
    "Pizzicato Strings",
    "Orchestral Strings",
    "Timpani",
    "String Ensemble 1",
    "String Ensemble 2",
    "SynthStrings 1",
    "SynthStrings 2",
    "Choir Aahs",
    "Voice Oohs",
    "Synth Voice",
    "Orchestra Hit",
    "Trumpet",
    "Trombone",
    "Tuba",
    "Muted Trumpet",
    "French Horn",
    "Brass Section",
    "SynthBrass 1",
    "SynthBrass 2",
    "Soprano Sax",
    "Alto Sax",
    "Tenor Sax",
    "Baritone Sax",
    "Oboe",
    "English Horn",
    "Bassoon",
    "Clarinet",
    "Piccolo",
    "Flute",
    "Recorder",
    "Pan Flute",
    "Blown Bottle",
    "Skakuhachi",
    "Whistle",
    "Ocarina",
    "Lead 1 (square)",
    "Lead 2 (sawtooth)",
    "Lead 3 (calliope)",
    "Lead 4 (chiff)",
    "Lead 5 (charang)",
    "Lead 6 (voice)",
    "Lead 7 (fifths)",
    "Lead 8 (bass+lead)",
    "Pad 1 (new age)",
    "Pad 2 (warm)",
    "Pad 3 (polysynth)",
    "Pad 4 (choir)",
    "Pad 5 (bowed)",
    "Pad 6 (metallic)",
    "Pad 7 (halo)",
    "Pad 8 (sweep)",
    "FX 1 (rain)",
    "FX 2 (soundtrack)",
    "FX 3 (crystal)",
    "FX 4 (atmosphere)",
    "FX 5 (brightness)",
    "FX 6 (goblins)",
    "FX 7 (echoes)",
    "FX 8 (sci-fi)",
    "Sitar",
    "Banjo",
    "Shamisen",
    "Koto",
    "Kalimba",
    "Bagpipe",
    "Fiddle",
    "Shanai",
    "Tinkle Bell",
    "Agogo",
    "Steel Drums",
    "Woodblock",
    "Taiko Drum",
    "Melodic Tom",
    "Synth Drum",
    "Reverse Cymbal",
    "Guitar Fret Noise",
    "Breath Noise",
    "Seashore",
    "Bird Tweet",
    "Telephone Ring",
    "Helicopter",
    "Applause",
    "Gunshot"};
#else
    "Piano a queue",
    "Piano de concert",
    "Piano electrique",
    "Piano de bastringue",
    "Piano electronique 1",
    "Piano electronique 2",
    "Clavecin",
    "Clavicorde",
    "Celesta",
    "Glockenspiel",
    "Boite a musique",
    "Vibraphone",
    "Marimba",
    "Xylophone",
    "Cloches (tubulaires)",
    "Tympanon (dulcimer)",
    "Orgue Hammond",
    "Orgue Hammond - effet percussion",
    "Orgue electronique",
    "Grand orgue",
    "Harmonium",
    "Accordeon",
    "Harmonica",
    "Bandoneon",
    "Guitare Acoustique (nylon)",
    "Guitare Acoustique (metal)",
    "Guitare electrique jazz",
    "Guitare electrique",
    "Guitare electrique amortie",
    "Guitare electrique saturee",
    "Guitare electrique a distorsion",
    "Harmoniques Guitare electrique",
    "Basse acoustique",
    "Basse electrique",
    "Basse electrique (mediator)",
    "Basse fretless",
    "Slap basse 1",
    "Slap basse 2",
    "Basse de synthese 1",
    "Basse de synthese 2",
    "Violon",
    "Alto (viole)",
    "Violoncelle",
    "Contrebasse",
    "Cordes Tremolo",
    "Cordes Pizzicato",
    "Harpe",
    "Timbales",
    "Ensemble cordes 1",
    "Ensemble cordes 2",
    "Cordes de synthese 1",
    "Cordes de synthese 2",
    "Choeurs Aahs",
    "Choeurs Oohs",
    "Choeurs de synthese",
    "Tutti d'orchestre symphonique",
    "Trompette",
    "Trombone",
    "Tuba",
    "Trompette sourdine",
    "Cor d'harmonie",
    "Ensemble de Cuivres",
    "Cuivres de synthese 1",
    "Cuivres de synthese 2",
    "Saxophone Soprano",
    "Saxophone Alto",
    "Saxophone tenor",
    "Saxophone baryton",
    "Haubois",
    "Cor Anglais",
    "Basson",
    "Clarinette",
    "Piccolo",
    "Flute traversiere",
    "Flute a bec",
    "Flute de pan",
    "Bouteille sifflee (cruche)",
    "Flute Shakuhachi",
    "Sifflet",
    "Ocarina",
    "Lead 1 (signal carre)",
    "Lead 2 (dent de scie)",
    "Lead 3 (orgue a vapeur, calliope)",
    "Lead 4 (chiff)",
    "Lead 5 (charang)",
    "Lead 6 (voix)",
    "Lead 7 (quintes)",
    "Lead 8 (basse + lead)",
    "Pad 1 (new age)",
    "Pad 2 (chaud)",
    "Pad 3 (synthetiseur)",
    "Pad 4 (choeurs)",
    "Pad 5 (bowed: archet, bande)",
    "Pad 6 (metallique)",
    "Pad 7 (halo)",
    "Pad 8 (sweep, balai)",
    "FX 1 (pluie)",
    "FX 2 (bande sonore)",
    "FX 3 (cristal)",
    "FX 4 (atmosphere)",
    "FX 5 (clarte)",
    "FX 6 (lutins)",
    "FX 7 (echos)",
    "FX 8 (science-fiction)",
    "Sitare",
    "Banjo",
    "Shamisen",
    "Koto",
    "Kalimba",
    "Cornemuse",
    "Fiddle (viole)",
    "Shanai",
    "Tintement de cloche",
    "Agogo",
    "Bidon (Steel band)",
    "Woodblock",
    "Tambour taiko",
    "Tom Melodique",
    "Percussion synthetique",
    "Son inverse de cymbale",
    "Bruit de Fret (guitare)",
    "Respiration",
    "Vagues",
    "Gazouillis",
    "Sonnerie de telephone",
    "Helicoptere",
    "Applaudissements",
    "Coup de feu"};
#endif
  if (n<0 || n>127)
    {
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "suspicious MIDI instrument"
#else
        "instrument MIDI suspect"
#endif
        " (%d)\n", fic_cur, n);
    return "???";
    }
  return tab[n];
  }


/* retourne le nom du controleur MIDI n */
char *controller(int n, char *s)
  {
  char *tab[]={
    "Bank Select MSB",
    "Modulation",
    "Breath control",
    "",
    "Foot pedal",
    "Portamento time",
    "Data Entry MSB",
    "Main Volume",
    "Balance",
    "",
    "Pan",
    "Expression",
    "Effect-type selector #1",
    "Effect-type selector #2",
    "",
    "",
    "General Purpose 1",
    "General Purpose 2",
    "General Purpose 3",
    "General Purpose 4",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Band Select LSB",
    "Modulation LSB",
    "Breath Control LSB",
    "",
    "Foot control LSB",
    "Portamento Time LSB",
    "Data Entry LSB",
    "Main Volume LSB",
    "Balance LSB",
    "",
    "Pan LSB",
    "Expression LSB",
    "",
    "",
    "",
    "",
    "General Purpose 1 LSB",
    "General Purpose 2 LSB",
    "General Purpose 3 LSB",
    "General Purpose 4 LSB",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sustain Pedal",
    "Portamento Pedal",
    "Sostenuto pedal",
    "Soft Pedal",
    "Legato pedal",
    "Hold 2",
    "Sound variation",
    "Harmonic content",
    "Release time",
    "Attack time",
    "Brightness",
    "Decay time",
    "Vibrato rate",
    "Vibrato depth",
    "Vibrato delay",
    "Undef./Special Effects",
    "General Purpose 5",
    "General Purpose 6",
    "General Purpose 7",
    "General Purpose 8",
    "Portamento Control",
    "",
    "",
    "",
    "",
    "",
    "",
    "Reverb depth",
    "Tremolo depth",
    "Chorus depth",
    "Celeste (detune) depth",
    "Phaser depth",
    "Data Increment (RPN+)",
    "Data Increment (RPN-)",
    "NRPN LSB",
    "NRPN MSB",
    "RPN LSB",
    "RPN MSB",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "All sounds off",
    "Reset all controllers",
    "Local control on/off",
    "All notes off",
    "Omni mode off",
    "Omni mode on",
    "Mono mode on",
    "Poly mode on"};
  char *p;
  if (mode_cur == GS)
    {
    tab[80]="GS Reverb program";
    tab[81]="GS Chorus program";
    }
  if (n<0 || n>127)
    {
    p="???";
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "suspicious MIDI controller"
#else
        "controleur MIDI suspect"
#endif
        " (%d)\n", fic_cur, n);
    }
  else if (n!=6 || !(p=find_nrpn()))
    p=tab[n];
  if (*p)
    sprintf(s, " (%s)", p);
  else
    *s=0;
  return s;
  }


/* decodage rudimentaire de SysEx (n: longueur de s[0] … '\xF7') */
char *decode_sysex(uchar *s, int n, bool *is_text)
  {
  static char result[80];
  struct
    {
    char is_text;
    char *sysex;
    char len;
    char *signification;
    } tab[]=
    {
    0,"\x7E\x7F\x09\x01\xF7", 5, "GM Reset",
    0,"\x7E\x00\x09\x01\xF7", 5, "SoundEngine GM on",
    0,"\x7E\x7F\x09\x02\xF7", 5, "GM off",
    0,"\x7F\x7F\x04\x01", 4, "Master volume",

    0,"\x41\x10\x42\x12\x40\x00\x7F\x00\x41\xF7", 10, "GS Reset",
    0,"\x41\x10\x42\x12\x40\x00\x7F\x7F\x42\xF7", 10, "GS off",
    0,"\x41\x10\x42\x12\x40\x00\x04", 7, "GS Master volume",
    0,"\x41\x10\x42\x12\x40\x01\x10", 7, "GS Reassigns voice reserves",
    0,"\x41\x10\x42\x12\x40\x01\x30\x00\x0F\xF7",10,"GS Reverb Room 1",
    0,"\x41\x10\x42\x12\x40\x01\x30\x01\x0E\xF7",10,"GS Reverb Room 2",
    0,"\x41\x10\x42\x12\x40\x01\x30\x02\x0D\xF7",10,"GS Reverb Room 3",
    0,"\x41\x10\x42\x12\x40\x01\x30\x03\x0C\xF7",10,"GS Reverb Hall 1",
    0,"\x41\x10\x42\x12\x40\x01\x30\x04\x0B\xF7",10,"GS Reverb Hall 2",
    0,"\x41\x10\x42\x12\x40\x01\x30\x05\x0A\xF7",10,"GS Reverb Plate",
    0,"\x41\x10\x42\x12\x40\x01\x30\x06\x09\xF7",10,"GS Reverb Short Delay",
    0,"\x41\x10\x42\x12\x40\x01\x30\x07\x08\xF7",10,"GS Reverb Panning Delay",
    0,"\x41\x10\x42\x12\x40\x01\x31", 7, "GS Reverb Depth",
    0,"\x41\x10\x42\x12\x40\x01\x32", 7, "GS Reverb Pre-LPF",
    0,"\x41\x10\x42\x12\x40\x01\x33", 7, "GS Reverb Level",
    0,"\x41\x10\x42\x12\x40\x01\x34", 7, "GS Reverb Time",
    0,"\x41\x10\x42\x12\x40\x01\x35", 7, "GS Reverb Delay",
    0,"\x41\x10\x42\x12\x40\x01\x36", 7, "GS Reverb Chorus level",
    0,"\x41\x10\x42\x12\x40\x01\x38\x00\x07\xF7",10,"GS Chorus 1",
    0,"\x41\x10\x42\x12\x40\x01\x38\x01\x06\xF7",10,"GS Chorus 2",
    0,"\x41\x10\x42\x12\x40\x01\x38\x02\x05\xF7",10,"GS Chorus 3",
    0,"\x41\x10\x42\x12\x40\x01\x38\x03\x04\xF7",10,"GS Chorus 4",
    0,"\x41\x10\x42\x12\x40\x01\x38\x04\x03\xF7",10,"GS Chorus Feedback",
    0,"\x41\x10\x42\x12\x40\x01\x38\x05\x02\xF7",10,"GS Chorus Flanger",
    0,"\x41\x10\x42\x12\x40\x01\x38\x06\x01\xF7",10,"GS Chorus Short Delay",
    0,"\x41\x10\x42\x12\x40\x01\x38\x07\x00\xF7",10,"GS Chorus Short Delay (Feedback)",
    0,"\x41\x10\x42\x12\x40\x01\x39", 7, "GS Chorus Pre-LPF",
    0,"\x41\x10\x42\x12\x40\x01\x3A", 7, "GS Chorus Level",
    0,"\x41\x10\x42\x12\x40\x01\x3B", 7, "GS Chorus Feedback",
    0,"\x41\x10\x42\x12\x40\x01\x3C", 7, "GS Chorus Delay",
    0,"\x41\x10\x42\x12\x40\x01\x3D", 7, "GS Chorus Rate",
    0,"\x41\x10\x42\x12\x40\x01\x3E", 7, "GS Chorus Depth",
    0,"\x41\x10\x42\x12\x40\x01\x3F", 7, "GS Chorus Ch.Send",
    0,"\x41\x10\x42\x12\x40\x01\x50\x00", 8, "GS ... Delay 1",
    0,"\x41\x10\x42\x12\x40\x01\x50\x01", 8, "GS ... Delay 2",
    0,"\x41\x10\x42\x12\x40\x01\x50\x02", 8, "GS ... Delay 3",
    0,"\x41\x10\x42\x12\x40\x01\x50\x03", 8, "GS ... Delay 4",
    0,"\x41\x10\x42\x12\x40\x01\x50\x04", 8, "GS ... PanDelay 1",
    0,"\x41\x10\x42\x12\x40\x01\x50\x05", 8, "GS ... PanDelay 2",
    0,"\x41\x10\x42\x12\x40\x01\x50\x06", 8, "GS ... PanDelay 3",
    0,"\x41\x10\x42\x12\x40\x01\x50\x07", 8, "GS ... PanDelay 4",
    0,"\x41\x10\x42\x12\x40\x01\x50\x08", 8, "GS ... DlyToRev",
    0,"\x41\x10\x42\x12\x40\x01\x50\x09", 8, "GS ... PanRepet",
    1,"\x41\x10\x42\x12\x40\x01\x00", 7, "GS Text",
    1,"\x41\x10\x45\x12\x10\x00\x00", 7, "GS Scrolling text",

    0,"\x43\x10\x4C\x00\x00\x7E\x00\xF7", 8, "XG System On",
    0,"\x43\x10\x4C\x00\x00\x7F\x00\xF7", 8, "XG Reset all",
    0,"\x43\x10\x4C\x02\x01\x00", 6, "XG Reverb type",
    0,"\x43\x10\x4C\x02\x01\x02", 6, "XG Reverb time",
    0,"\x43\x10\x4C\x02\x01\x03", 6, "XG Diffusion",
    0,"\x43\x10\x4C\x02\x01\x04", 6, "XG Initial delay",
    0,"\x43\x10\x4C\x02\x01\x05", 6, "XG HPF cutoff frequency",
    0,"\x43\x10\x4C\x02\x01\x06", 6, "XG LPF cutoff frequency",
    0,"\x43\x10\x4C\x02\x01\x0B", 6, "XG Dry/Wet",
    0,"\x43\x10\x4C\x02\x01\x0C", 6, "XG Reverb return level",
    0,"\x43\x10\x4C\x02\x01\x0D", 6, "XG Reverb pan",
    0,"\x43\x10\x4C\x02\x01\x12", 6, "XG Er/RevBal",
    0,"\x43\x10\x4C\x02\x01\x20", 6, "XG Chorus type",
    0,"\x43\x10\x4C\x02\x01\x22", 6, "XG LFO frequency",
    0,"\x43\x10\x4C\x02\x01\x23", 6, "XG LFO PM depth",
    0,"\x43\x10\x4C\x02\x01\x24", 6, "XG FB level",
    0,"\x43\x10\x4C\x02\x01\x25", 6, "XG Delay offset",
    0,"\x43\x10\x4C\x02\x01\x2C", 6, "XG Chorus return level",
    0,"\x43\x10\x4C\x02\x01\x2D", 6, "XG Chorus pan",
    0,"\x43\x10\x4C\x02\x01\x2E", 6, "XG Send chorus to reverb",
    0,"\x43\x10\x4C\x02\x01\x40", 6, "XG Variation type",
    0,"\x43\x10\x4C\x02\x01\x56", 6, "XG Variation return level",
    0,"\x43\x10\x4C\x02\x01\x57", 6, "XG Variation pan",
    0,"\x43\x10\x4C\x02\x01\x58", 6, "XG Send variation to reverb",
    0,"\x43\x10\x4C\x02\x01\x59", 6, "XG Send variation to chorus",
    0,"\x43\x10\x4C\x02\x01\x5A", 6, "XG Variation connection",
    0,"\x43\x10\x4C\x02\x01\x5B", 6, "XG Variation send",
    0,"\x43\x10\x4C\x02\x40\x00", 6, "XG EQ type",
    0,"\x43\x10\x4C\x02\x40\x01", 6, "XG 80 Hz",
    0,"\x43\x10\x4C\x02\x40\x05", 6, "XG 500 Hz",
    0,"\x43\x10\x4C\x02\x40\x09", 6, "XG 1.0 kHz",
    0,"\x43\x10\x4C\x02\x40\x0D", 6, "XG 4.0 kHz",
    0,"\x43\x10\x4C\x02\x40\x11", 6, "XG 8.0 kHz",
    1,"\x43\x10\x4C\x06\x00\x00", 6, "XG Scrolling text",
    0,"\x43\x10\x4C\x08\x00\x05", 6, "XG Poly/mono",
    0,"\x43\x10\x4C\x08\x00\x07", 6, "XG Part mode",
    0,"\x43\x10\x4C\x08\x00\x0C", 6, "XG Vel sens depth",
    0,"\x43\x10\x4C\x08\x00\x0D", 6, "XG Vel sens offset",
    0,"\x43\x10\x4C\x08\x00\x0F", 6, "XG Note limit low",
    0,"\x43\x10\x4C\x08\x00\x10", 6, "XG Note limit high",
    0,"\x43\x10\x4C\x08\x00\x15", 6, "XG Vibrato rate",
    0,"\x43\x10\x4C\x08\x00\x16", 6, "XG Vibrato depth",
    0,"\x43\x10\x4C\x08\x00\x17", 6, "XG Vibrato delay",
    0,"\x43\x10\x4C\x08\x00\x19", 6, "XG Resonance",
    0,"\x43\x10\x4C\x08\x00\x1A", 6, "XG Attack time",
    0,"\x43\x10\x4C\x08\x00\x1B", 6, "XG Decay time",
    0,"\x43\x10\x4C\x08\x00\x1C", 6, "XG Release time",
    0,"\x43\x10\x4C\x08\x00\x20", 6, "XG MW LFO mod",
    0,"\x43\x10\x4C\x08\x00\x23", 6, "XG Pitch bend control",
    0,"\x43\x10\x4C\x08\x00\x67", 6, "XG Portamento sw",
    0,"\x43\x10\x4C\x08\x00\x68", 6, "XG Portamento time",
    0,"\x43\x10\x4C\x08\x00\x69", 6, "XG Pitch EG init level",
    0,"\x43\x10\x4C\x08\x00\x6A", 6, "XG Pitch EG attack time",
    0,"\x43\x10\x4C\x08\x00\x6B", 6, "XG Pitch EG release level",
    0,"\x43\x10\x4C\x08\x00\x6C", 6, "XG Pitch EG release time",
    0,"\x43\x10\x4C\x08\x00\x6D", 6, "XG Vel limit low",
    0,"\x43\x10\x4C\x08\x00\x6E", 6, "XG Vel limit high",

    0,"\x41\x10\x6A\x12", 4, "Roland JV/XP...",
    0,"\x41\x10\x16\x12", 4, "Roland D-110...",
    0,"\x41\x10\x00\x10", 4, "Roland XV-5080...",       /* ??? */
    0,"\x41\x10\x42\x12\x00\x00\x7F", 7, "Roland SC-88",
    0,"\x43\x71\x7E\x09\x01\xF7", 6, "Yamaha TG300",
    0,"\x00\x00\x1B", 3, "Peavey",
    };
  register i;
  for (i=0; i<sizeof(tab)/sizeof(tab[0]); i++)
    if (memcmp(s, tab[i].sysex, min(n, tab[i].len)) == SUCCESS)
      {
      *is_text = (tab[i].is_text ? TRUE : FALSE);
      if (strcmp(tab[i].signification, "GM Reset") == SUCCESS)
        mode_cur = GM2;
      else if (strcmp(tab[i].signification, "GS Reset") == SUCCESS ||
               strstr(tab[i].signification, "Roland"))
        mode_cur = GS;
      else if (strcmp(tab[i].signification, "XG System On") == SUCCESS)
        mode_cur = XG;
      return tab[i].signification;
      }
  *is_text = FALSE;
  if (*s == 0x41)       /* Roland */
    {
    strcpy(result, "GS");
    if (memcmp(s, "\x41\x10\x42\x12\x40", 5) == SUCCESS)
      {
      if (s[5]>='\x10' && s[5]<='\x1F')
        switch(s[6])
          {
          case '\x02':
            sprintf(result, "GS Receive channel/part turn %s %d", s[7]==16?"off":"on", gspart(s[5]&0x0F));
            break;
          case '\x14':  /* single, limited multi, full multi */
            sprintf(result, "GS Assign mode %02d ch.%d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x15':
            if (s[7]==2 || s[7]==1)     /* Drum Map 2 */
              sprintf(result, "GS Assign 2nd %s kit ch.%d",
                s[7]==2 ? "Drum" : "Voice"/*???*/,
                gspart(s[5]&0x0F));
            break;
          case '\x19':
            sprintf(result, "GS Turn volume %s part %d", s[7]?"on":"off", gspart(s[5]&0x0F));
            break;
          case '\x1C':
            sprintf(result, "GS Set pan of part %d to random", gspart(s[5]&0x0F));
            break;
          case '\x30':
            sprintf(result, "GS Vibrato Rate ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x31':
            sprintf(result, "GS Vibrato Rate ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x32':
            sprintf(result, "GS Cutoff Freq ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x33':
            sprintf(result, "GS Resonance ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x34':
            sprintf(result, "GS Envelope Attack ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x35':
            sprintf(result, "GS Envelope Delay ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x36':
            sprintf(result, "GS Envelope Release ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x37':
            sprintf(result, "GS Vibrato Delay ch.%d %d", gspart(s[5]&0x0F), s[7]);
            break;
          case '\x40':
            /* n==21 */
            sprintf(result, "GS Scale tuning ch.%d", gspart(s[5]&0x0F));
            break;
          }
      if (s[5]>='\x20' && s[5]<='\x2F')
        if (s[6] >= '\x00' && s[6] <= '\x06')
          /* Pitch control off, TVF cutoff, amplitude, LFO1 rate, LFO1 pitch depth, LFO1 TVF depth, LFO1 TVA depth */
          sprintf(result, "GS Mod tuning ch.%d", gspart(s[5]-0x20));
        else if (s[6] >= '\x10' && s[6] <= '\x16')
          sprintf(result, "GS Bend tuning ch.%d", gspart(s[5]-0x20));
        else if (s[6] >= '\x20' && s[6] <= '\x26')
          sprintf(result, "GS CAF tuning ch.%d", gspart(s[5]-0x20));
        else if (s[6] >= '\x40' && s[6] <= '\x4A')
          sprintf(result, "GS CC1 tuning ch.%d", gspart(s[5]-0x20));
        else if (s[6] >= '\x50' && s[6] <= '\x5A')
          sprintf(result, "GS CC2 tuning ch.%d", gspart(s[5]-0x20));
      /* d'apres http://www.atticmf.com/jishu/doc_detail.php?id=353 */
      }
    if (opt.curieux && strcmp(result, "GS") == SUCCESS)
nrsys:
      fprintf(stderr, "%s: "
#ifdef ENG
        "non registered %s SysEx"
#else
        "code SysEx %s non repertorie"
#endif
        "\n", fic_cur, result);
    return result;
    }
  else if (*s == 0x42)  /* Korg */
    {
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "Korg SysEx"
#else
        "code SysEx Korg"
#endif
        "\n", fic_cur);
    return "Korg";
    }
  else if (*s == 0x7E)
    {
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
        "Non real time SysEx"
#else
        "code SysEx temps non reel"
#endif
        "\n", fic_cur);
    return "Non real time";
    }
  else if (*s == 0x43)  /* Yamaha */
    {
    strcpy(result, "XG");
    if (n==8 && memcmp(s, "\x43\x10\x4C", 3) == SUCCESS && s[3]>='\x30' && s[3]<='\x35')
      sprintf(result, "XG Modify drum part %d note %d", s[3]-'\x30', s[4]);
    else if (n==8 && s[1]>=0x11 && s[1]<=0x1F
        && memcmp(s+2, "\x4C\x00\x00\x7E\x00\xF7", 6) == SUCCESS)
      {
      sprintf(result, "XG System On (port %d)", s[1]-0x10);
      mode_cur = XG;    /* tolerance... */
      }
    else if (n==9 && memcmp(s, "\x43\x10\x4C\x02\x01", 5) == SUCCESS
        && s[5]>='\x42' && s[5]<='\x54' && s[5]%2==0)
      sprintf(result, "XG Variation parameter %d", (s[5]-0x40)/2);
    else if (n==8 && memcmp(s, "\x43\x10\x4C\x08", 4) == SUCCESS)
      switch(s[5])
        {
        case 0x01:
          sprintf(result, "XG Bank Select MSB ch.%d %d", s[4], s[6]);
          bank[s[4]][0] = s[6];
          break;
        case 0x02:
          sprintf(result, "XG Bank Select LSB ch.%d %d", s[4], s[6]);
          bank[s[4]][1] = s[6];
          break;
        case 0x03:
          sprintf(result, "XG Instrument ch.%d %d: %s bank %d %d",
                s[4], s[6], instrument(s[6]),
                bank[s[4]][0], bank[s[4]][1]);
          break;
        case 0x04:
          sprintf(result, "XG Rcv channel ch.%d %d", s[4], s[6]);
          break;
        case 0x05:
          sprintf(result, "XG Poly/mono ch.%d %d", s[4], s[6]);
          break;
        case 0x07:
          sprintf(result, "XG Part mode ch.%d %d", s[4], s[6]);
          break;
        case 0x08:
          sprintf(result, "XG Note shift ch.%d %d", s[4], s[6]);
          break;
        case 0x09:
          sprintf(result, "XG Detune ch.%d %d", s[4], s[6]);
          break;
        case 0x0B:
          sprintf(result, "XG Volume ch.%d %d", s[4], s[6]);
          break;
        case 0x0C:
          sprintf(result, "XG Vel sens depth ch.%d %d", s[4], s[6]);
          break;
        case 0x0E:
          sprintf(result, "XG Pan ch.%d %d", s[4], s[6]);
          break;
        case 0x11:
          sprintf(result, "XG Dry level ch.%d %d", s[4], s[6]);
          break;
        case 0x12:
          sprintf(result, "XG Chorus send ch.%d %d", s[4], s[6]);
          break;
        case 0x13:
          sprintf(result, "XG Reverb send ch.%d %d", s[4], s[6]);
          break;
        case 0x14:
          sprintf(result, "XG Variation send ch.%d %d", s[4], s[6]);
          break;
        case 0x15:
          sprintf(result, "XG Vibrato rate ch.%d %d", s[4], s[6]);
          break;
        case 0x16:
          sprintf(result, "XG Vibrato depth ch.%d %d", s[4], s[6]);
          break;
        case 0x17:
          sprintf(result, "XG Vibrato delay ch.%d %d", s[4], s[6]);
          break;
        case 0x18:
          sprintf(result, "XG Filter cutoff ch.%d %d", s[4], s[6]);
          break;
        case 0x19:
          sprintf(result, "XG Filter resonance ch.%d %d", s[4], s[6]);
          break;
        case 0x1A:
          sprintf(result, "XG Attack time ch.%d %d", s[4], s[6]);
          break;
        case 0x1B:
          sprintf(result, "XG Decay time ch.%d %d", s[4], s[6]);
          break;
        case 0x1C:
          sprintf(result, "XG Release time ch.%d %d", s[4], s[6]);
          break;
        case 0x1D:
          sprintf(result, "XG MW pitch control ch.%d %d", s[4], s[6]);
          break;
        case 0x1E:
          sprintf(result, "XG MW filter control ch.%d %d", s[4], s[6]);
          break;
        case 0x1F:
          sprintf(result, "XG MW amplitude control ch.%d %d", s[4], s[6]);
          break;
        case 0x20:
          sprintf(result, "XG MW LFO mod ch.%d %d", s[4], s[6]);
          break;
        case 0x41:
          sprintf(result, "XG Scale tuning C ch.%d %d", s[4], s[6]);
          break;
        case 0x69:
          sprintf(result, "XG Pitch EG init level ch.%d %d", s[4], s[6]);
          break;
        case 0x6A:
          sprintf(result, "XG Pitch EG attack time ch.%d %d", s[4], s[6]);
          break;
        case 0x6B:
          sprintf(result, "XG Pitch EG release level ch.%d %d", s[4], s[6]);
          break;
        case 0x6C:
          sprintf(result, "XG Pitch EG release time ch.%d %d", s[4], s[6]);
          break;
        default:
          sprintf(result, "XG) (??? %02X ch.%d %d", s[5], s[4], s[6]);
          break;
        }
    else if (s[2]=='\x4C')
      {
      if (s[1]==0)
        strcpy(result, "XG Bulk Dump");
      else if ((s[1] & 0xF0) == 0x20 && n == 6)
        strcpy(result, "XG Dump Request");
      else if ((s[1] & 0xF0) == 0x30 && n == 6)
        strcpy(result, "XG Parameter Request");
      }
    if (opt.curieux && strcmp(result, "XG") == SUCCESS)
      goto nrsys;
    return result;
    }
  if (opt.curieux)
    fprintf(stderr, "%s: "
#ifdef ENG
        "unknown SysEx"
#else
        "code SysEx inconnu"
#endif
        " (Manufacturer ID=%02X)\n",
        fic_cur, *s);
  *is_text = isprint(*s);
  return 0;
  }


/* taille d'un fichier */
long file_length(FILE *f)
  {
  long pos0=ftell(f), pos;
  fseek(f, 0L, SEEK_END);
  pos=ftell(f);
  fseek(f, pos0, SEEK_SET);
  return pos;
  }


/* trouve les tempos concernes par t, dt; retourne le 1er et nombre dans *n */
TEMPO *get_tempo(ulong t, ulong dt, int *n)
  {
  register int i;
  int n1,n2;
  ushort ntem2 = min(ntem, MAX_TEM);

  n1=0;
  for (i=1; i<ntem2; i++)
    {
    if (ttem[i].t > t)
      {
      n1=i-1;
      break;
      }
    }
  if (i==ntem2)
    n1 = ntem2-1;
  n2=n1;
  for (i=n1+1; i<ntem2; i++)
    {
    if (ttem[i].t >= t+dt)
      {
      n2=i-1;
      break;
      }
    }
  if (i==ntem2)
    n2 = ntem2-1;
  *n = n2-n1+1;
  return ttem+n1;
  }


/* affiche le code Ascii de chaque octet du fichier (entre last_pos et pos) */
void hexdump(FILE *f)
  {
  static long n;
  unsigned char buf[256];
  long pos, len, n_read;
  register short i,k;

  if (last_pos == 0)
    n=0;
  pos = ftell(f);
  len = (short)(pos - last_pos);
  n += len;
  fseek(f, -len, SEEK_CUR);
  n_read = 0;
  for (i=0; i<len; i++)
    {
    k = i % sizeof(buf);
    if (k == 0)
      {
      short chunk;
      if (len - n_read < sizeof(buf))
        chunk = (short)(len - n_read);
      else
        chunk = sizeof(buf);
      n_read += fread(buf, 1, chunk, f);
      }
    if (i%26 == 0)
      {
      if (i>0)
        printf("|\n");
      printf("|");
      }
    else
      printf(" ");
    printf("%02X", buf[k]);
    }
  if (opt.hexa)
    printf("| (%05lX)\n", n);
  else
    printf("| (%ld)\n", n);
  last_pos = pos;
  }


unsigned char gspart(unsigned char p)
  {
  if (p>=1 && p<=9)
    return p;
  if (p==0)
    return 10;
  return p+1;
  }


char *find_nrpn()
  {
  static char s[80];
  char t[40];
  bool found=TRUE;
  strcpy(s, "Data Entry MSB: ");
  if (nrpn[canal][0]==0)
    {
    if (nrpn[canal][1]==0)
      strcat(s, "Pitch Bend range (1/2)");
    else if (nrpn[canal][1]==1)
      strcat(s, "Fine Tuning (1/100)");
    else if (nrpn[canal][1]==2)
      strcat(s, "Coarse Tuning (1/2)");
    else
      found=FALSE;
    }
  else if (nrpn[canal][0]==1)
    {
    if (nrpn[canal][1]==8)
      strcat(s, "Vibrato rate");
    else if (nrpn[canal][1]==9)
      strcat(s, "Vibrato depth");
    else if (nrpn[canal][1]==10)
      strcat(s, "Vibrato delay");
    else if (nrpn[canal][1]=='\x20')
      strcat(s, "Filter cutoff freq");
    else if (nrpn[canal][1]=='\x21')
      strcat(s, "Filter resonance freq");
    else if (nrpn[canal][1]=='\x63')
      strcat(s, "EG Attack time freq");
    else if (nrpn[canal][1]=='\x64')
      strcat(s, "EG Decay time freq");
    else if (nrpn[canal][1]=='\x66')
      strcat(s, "EG Release time freq");
    else
      found=FALSE;
    }
  else if (nrpn[canal][0] == '\x14')
    {
    sprintf(t, "Drum %d Filter cutoff freq.", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x15')
    {
    sprintf(t, "Drum %d Filter Resonance", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x16')
    {
    sprintf(t, "Drum %d EG Decay time", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x17')
    {
    sprintf(t, "Drum %d EG Decay rate", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x18')
    {
    sprintf(t, "Pitch of drum %d (1/2)", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x19')
    {
    sprintf(t, "Pitch of drum %d (1/100)", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x1A')
    {
    sprintf(t, "Level of drum %d", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x1C')
    {
    sprintf(t, "Pan of drum %d", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x1D')
    {
    sprintf(t, "Reverb of drum %d", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x1E')
    {
    sprintf(t, "Chorus of drum %d", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x1F')
    {
    sprintf(t, "Variation of drum %d", nrpn[canal][1]);
    strcat(s, t);
    }
  else if (nrpn[canal][0] == '\x7F' && nrpn[canal][1] == '\x7F')
    {
    strcat(s, "NULL");
    if (opt.curieux)
      fprintf(stderr, "%s: NRPN nul"
#ifdef ENG
                "l"
#endif
                "\n" , fic_cur);
    }
  else
    found=FALSE;

  if (! found)
    {
    if (opt.curieux)
      fprintf(stderr, "%s: "
#ifdef ENG
                "Non registered NRPN"
#else
                "NRPN non repertorie"
#endif
                " (%02X%02Xh)\n", fic_cur,
                nrpn[canal][0], nrpn[canal][1]);
    /* return 0; */
    sprintf(t, "%02X%02Xh ???", nrpn[canal][0], nrpn[canal][1]);
    strcat(s,t);
    }
  return s;
  }
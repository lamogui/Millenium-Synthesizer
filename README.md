Aperçus
=======

![](https://github.com/lamogui/Millenium-Synthesizer/raw/master/mis/img/NouvelleInterface.png "NELead6 (inspiré du nord lead 3")

![](https://github.com/lamogui/Millenium-Synthesizer/raw/master/mis/img/capture%20puresquare.png "Pure Square Synth")




Installation
============

Releases
-------
Quand une version du programme est potable est compilée sous windows elle est disponible
en release ici :
 - [Millenium Synthesizer Win32](http://bdene666.site90.net/PRO_RELEASES/MiLLENiUM_SYNTH_WIN32.zip)
 - [Millenium Synthesizer Win64](http://bdene666.site90.net/PRO_RELEASES/MiLLENiUM_SYNTH_WIN64.zip)
 
 
Avec ces versions il suffit de décompresser et d'utiliser ! ;)


 
Sinon cloner le git (ou télécharger le zip de l'état actuel)

Windows
-------
 - 64 bits : copiez bass.dll et bassasio.dll depuis le repertoire win64 vers 
   la racine du projet et lancez Synthesizer Millenium x64.exe
    
 - 32 bits : copiez bass.dll et bassasio.dll depuis le repertoire win32 vers 
   la racine du projet et lancez Synthesizer Millenium.exe
   
Attention si vous recompilez le projet une version modifié (hacké) de la sfml est utilisé 
(incluse dans libwin32 et libwin64) La compilation avec la version officielle fonctionnera
mais le redimensionnement de la fenetre sera pénible.
   
Linux
-----

 - 32 bits [JAMAIS TESTE]  : compilez le projet avec la commande "make linux32" 
   puis lancez "./test.x32". Vous devez avoir la version officielle (non compatible ENIB)
   de la SFML 2.1 ainsi que toutes ces dépendances (OpenGL, X11, Xrand, freetype)
 
 - 64 bits : compilez avec la commande "make linux64" puis lancez "./test.x64" 
   Vous devez avoir la version officielle (non compatible ENIB) de la SFML 2.1 
   
 - 64 bits ENIB : lancez la commande "make linux64enib" puis lancez "./test.enib"
   Attention la version 2.1 statique sans support des fonts (sans freetype) est utilisé 
   lors de la compilation. Elle est incluse dans le répértoire liblinux64/ENIB 
   Plus d'infos sur cette version modifié de la SFML 2.1 ici https://github.com/lamogui/SFML_2.1_ENIB
   
Sous linux le son semble bugué (c'est ennuyeux c'est vrai).
 
MACOSX
------

 Désolé d'être pauvre Trololol
 
Utilisation
===========

Modifiez les paramètres à l'aide des potards (souris) et appuez sur les touches : QZSEDFTGYHUJKOLPM 
pour jouer les différentes notes. Les instruments sont polyphoniques il est donc possible de jouer 
plusieurs notes en même temps.

Configuration
=============

Le projet à un fichier de configuration nommé **settings.ini**
Il suffit de modifier les valeurs des paramètre à la main via un
bloc note.

Driver ASIO
-----------
Si vous avez un driver ASIO (sous windows) comme par exemple ASIO4ALL il est possible de l'utiliser
en changeant la ligne UseASIODriver=False; en True...
Utiliser un driver ASIO diminue fortement les "bugs sonores" (car le driver BASS est plus lent)


FFT 
---
Par défaut le programme est réglé sur une FFT qui utilise beaucoup d'échantillons, 
Cela peut causer des lags si vous utilisez à la fois : la FFT, des Releases Times très long
combinés à la lecture d'un fichier MIDI.
Vous pouvez augmenter les performances en diminuant le nombre d'échantillons :
Un exemple qui marche bien.

Samples = 4096;

Notes qu'il faut que la valeur soit une puissance de 2 codé sur un nombre de bit paire
(Il y a un bug avec les nombres de bit impairs)


Repertoires 
===========

 - build : contient les fichiers compilés par mingw/gcc
 - img : contient les textures et captures d'écrans
 - fonts : contient les fonts utilisés par le projet
 - musics : contients quelques musiques au format MIDI 
 - presets : contients des presets pour l'instrument NELEAD6
 - include : contient les déclarations (hpp) et les en-têtes des librairies
 - libXXXX : contient les librairies pour la platforme XXXX
 - mis : contient l'UML
 - src : contient les fichiers sources du projet
 - visual : contient les fichiers pour compiler avec visual studio 11 (2012) en 64bits.
 
 

UML et MODELIO
==============
Les fichiers concernant la MIS sont dans le repertoire "mis" :
 - Les dossiers des différentes versions du projet (Uniquement V1 et V1.1 pour le moment)
 - Fichiers Modelio les plus récents

Pour ouvrir l'uml il vous faut : Modelio 3.1.0, ensuite cliquez sur 
File -> Switch Workspace -> Selectionner le dossier workspace dans 
"Documents\Github\POO\mis\workspace"

Les fichiers modelio ne contienne pas les diagrammes UML pour une version spécifique... 
Consultez les dossiers pour cela...


Attention ! Le code est actuellement en version 1.7 certaines classes ont été rajoutés par 
rapport à l'UML de la V1.1 mais le fonctionnement des classes présentes dans la V1.1 reste 
le même (il y a uniquement eu des ajouts)



Textures
========

Background : Gabriel Gajdoš (http://pipper-svk.deviantart.com/art/Hyperdrive-411313660)


S'impliquer dans le projet
==========================

Vous voulez coder quelquechose pour le projet ? permettez moi de vous rediriger vers 
ce merveilleux [wiki](https://github.com/lamogui/Millenium-Synthesizer/wiki) pour savoir où commencer... Une fois que vous maitrisez le code 
vous pouvez par exemple faire un TODO voir ci-dessous
Procédez par un classique Fork puis Pull Request et je vous ajouterai sur les contributeurs



TODO
====

 - Coder le Fader
 - **Réecrire le code qui lit les fichiers midi**
 - **fixer une convention sur les notations des méthodes et attributs et utiliser la même PARTOUT**
 - Indenter/commenter 
 - Finir le NELead6 
 - Coder d'autres drivers
 - Coder la méthode bool readSignal(Signal& signal) dans AudioStream, et regarder ce que l'on peut faire avec SoundStream de la SFML.
 - Faire un fichier instrument.cpp

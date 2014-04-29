Aperçus
=======

![](https://github.com/lamogui/Millenium-Synthesizer/raw/master/mis/img/capture%20puresquare.png "Pure Square Synth")

![](https://github.com/lamogui/Millenium-Synthesizer/raw/master/mis/img/capture%20inro.png "NELead6 (inspiré du nord lead 3")


Installation
============

Tout d'abord cloner le git (ou télécharger le zip de l'état actuel)

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

Repertoires 
===========

 - build : contient les fichiers compilés par mingw/gcc
 - img : contient les textures et captures d'écrans
 - include : contient les déclarations (hpp) et les en-têtes des librairies
 - libXXXX : contints les librairies pour la platforme XXXX
 - mis : contient lUML
 - src : contient les fichiers sources du projet
 - visual : contient les fichiers pour compiler avec visual studio 11 (2012) en 64bits.
 
 

UML et MODELIO
==============
Les fichiers concernant la MIS sont dans le repertoire "mis" :
 - Scénario.doc 
 - Fichiers Modelio

Pour faire l'uml il vous faut : Modelio 3.1.0, ensuite cliquez sur 
File -> Switch Workspace -> Selectionner le dossier workspace dans 
"Mes Documents\Github\POO\mis\workspace"


S'impliquer dans le projet
==========================

Vous voulez coder quelquechose pour le projet ? permettez moi de vous rediriger vers 
ce merveilleux [wiki](https://github.com/lamogui/Millenium-Synthesizer/wiki) pour savoir où commencer... Une fois que vous maitrisez le code 
vous pouvez par exemple faire un TODO voir ci-dessous
Procédez par un classique Fork puis Pull Request et je vous ajouterai sur les contributeurs



TODO
====

 - Coder les oscillateurs : random, smooth random, noise et triple sin
 - Coder le Fader et le Button (dans interface.hpp/cpp)
 - Faire le code qui lit les fichiers midi
 - Faire un fichier instrument.cpp
 - Coder les getters/setters
 - Finir le NELead6
 - Coder d'autres drivers
 - Mettre des cartouches avec des ASCII Art (lol)
 - Coder la méthode bool readSignal(Signal& signal) dans AudioStream, et regarder ce que l'on peut faire avec SoundStream de la SFML.

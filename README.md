Aperçus
=======

![](https://raw.githubusercontent.com/lamogui/POO/master/mis/img/capture%20puresquare.png "Pure Square Synth")

![](https://raw.githubusercontent.com/lamogui/POO/master/mis/img/capture%20inro.png "NELead6 (inspiré du nord lead 3")


UML et MODELIO
==============
Les fichiers concernant la MIS sont dans le repertoire "mis" :
 - Scénario.doc 
 - Fichiers Modelio

Pour faire l'uml il vous faut : Modelio 3.1.0, ensuite cliquez sur 
File -> Switch Workspace -> Selectionner le dossier workspace dans 
"Mes Documents\Github\POO\mis\workspace"

Attention ne pas faire de commit si vous avez tout peter ! je ne sait pas 
si on peut faire un reserve des fichiers binaires...



Petite Méga explications des classes
====================================

Signal (signal.hpp)
-------------------

Unité de "son", tout son continu est décomposé en signaux (un tableau 
d'échantillons), c'est ce qu'un instrument doit générer. 

Oscillator (oscillator.hpp)
---------------------------
Générateur de signal périodique il genere un signal grâce à la méthode 
"generate", mais il est aussi possible de generer le signal sur un objet Signal 
exterieur grâce à "step". (TODO: Coder les oscillateurs : random, smooth random
noise et triple sin)

InstrumentVoice (instrument.hpp)
--------------------------------
C'est un générateur de signal (comme un oscillateur) mais il est lié à une Note 
(ET UNE SEULE NOTE !), le signal peut ne pas être périodique (et d'ailleurs ne 
doit pas l'être). 

Instrument (instrument.hpp)
---------------------------
C'est un template, il regroupe plusieurs InstrumentVoice, et affecte chacuns 
d'entre eux à une note qu'il reçoit. Il fait également le mixage final entre 
tous les signaux générés par ces InstrumentVoice. (TODO : faire un fichier 
instrument.cpp)  

AbstractInstrument (instrument.hpp)
-----------------------------------
C'est la base d'un Instrument, elle permet a un gestionnaire d'instrument de 
gérer les différents Instrument sans connaître les InstrumentVoice associés 
(en effet la classe instrument est une classe template). La classe Instrument 
a déjà un minimum de code fournis (gestion des notes !) alors héritez de la 
classe Instrument et PAS de cette classe ! (sauf si vous voulez faire 
quelque chose de complétement différent)

Note (note.hpp)
---------------
Représente une note de musique : fréquence, vélocité, durée mais aussi quand 
elle à été joué ! Quand une note est créée elle est envoyé à l'Instrument 
(via la méthode playNote(Note & n)) qui l'affectera si il le peut à un 
InstrumentVoice via la méthode beginNote(Note& n) qui lui même préviendra 
la note via receivePlayedSignal(InstrumentVoice* v). Lorsqu'elle est terminé 
la note doit prévenir l'InstrumentVoice (méthode sendStopSignal()) afin que 
celui-ci démarre son Release Time (méthode endNote()) ainsi une note peut 
être terminé mais l'InstrumentVoice continue d'être "utilisé" tant qu'il n'as 
pas fini son release time. 

InstrumentParameter (note.hpp)
------------------------------
Représente un paramètre de l'instrument codé sur un entier signé de 16 bits.
A l'initialisation l'instrument donne la plage de valeur (que le paramètre 
ne doit pas depasser), ensuite différentes méthodes peuvent modifier cette 
valeur en utilisant une autre plage de valeurs (par exemple 
setValueFromUnsigned), InstrumentParameter sert à faire la conversion... 
(TODO : coder les getters/setters utiles, coder une méthode avec des 
flottants)


AudioStream (audiostream.hpp)
-----------------------------
Représente un flux, le son continu qui va sortir des haut parleurs, Il s'agit 
d'un buffer circulaire de type FIFO (c'est quasiment un copier coller du TP6 
S5A d'asm), Le truc qu'il a en plus c'est qu'il est verrouillable et 
deverrouillable (méthodes lock() et unlock()) pour éviter que 2 threads 
l'utilise en même temps. Ah oui et il peut aussi ajouter directement un 
Signal entier. (TODO : coder la méthode bool readSignal(Signal& signal), 
et regarder ce que l'on peut faire avec SoundStream de la SFML)


AudioDriver (audiodriver.hpp)
-----------------------------
Représente le lien avec le Haut Parleur lors de son démarrage (méthode start) 
on lui passe un AudioStream qu'il devra lire (via la méthode read) lorsque la 
carte son à besoin de nouveaux samples. Pour l'instant il y a que 2 drivers 
possibles : BASS et BASSASIO (BASSASIO requiert windows et un driver asio 
d'installé (par exemple ASIO4ALL). (TODO mineur: Coder d'autres classes 
driver à l'aide d'autres librairies)


MouseCatcher (interface.hpp)
----------------------------
Représente un élément de l'interface graphique qui utilise la souris. 
Dans un premier temps l'utilisateur clique sur l'élément (exemple potard)
(méthode onMousePress) ce qui à pour effet "d'attraper la souris". 
Une fois qu'un MouseCatcher à attrapé la souris, il faut attendre que 
l'utilisateur relache le bouton de la souris pour en attraper un nouveau.
C'est la fonction main qui s'en charge pour le moment.
Un MouseCatcher doit être dessinable !


Knob (interface.hpp)
--------------------
Knob veut dire potard en anglais. Cette classe est un MouseCatcher 
Elle doit être lié à un InstrumentParameter pour agir directement dessus.
(TODO: coder les getters/setters pour l'attribut InstrumentParameter)

ScrollBar (interface.hpp)
-------------------------
Permet de gérer la vue d'une Interface si elle est trop petite pour afficher
l'integralité de l'Interface. (TODO gérér les bugs quand la sf::View est plus 
grande que l'Interface)


Interface (interface.hpp)
-------------------------
Voilà la classe qui justifie le nom du fichier, Interface gére des 
sf::Drawable (choses affichables) et des MouseCatcher. Elle s'occupe de les
faire afficher dans l'espace que l'on lui donne ! (TODO : Getters/setters 
trucs utiles) 



Variant (variant.hpp)
--------------------
Représente quelque chose ! soit un string, soit un bool soit un nombre ! 
(TODO mineur: clean et optimisations)

Settings (settings.hpp)
-----------------------
Veille Classe Singleton codé il y a longtemps, permet de charger des paramètre 
depuis un fichier (regarder la fonction main pour voir comment ça marche c'est 
tout con), comme elle ne sait pas de quel type est le paramètre elle utilise 
des Variant (TODO mineur: clean et optimisations)


Voilà j'ai fait le tour... Il faudrait faire l'UML...

D'autres TODO
=============
 - NoteTranslator : une classe qui traduit quelque chose (par exemple touches 
 de clavier) en Notes !
 - "Music" : une classe qui lit/écrit des notes dans un fichier et 
 instancie/envoie les notes aux instruments
 - Button et Fader héritant de MouseCatcher
 - KeyboardInterface : affiche un clavier et envois les notes appuyés à la 
 classe Instrument associé
 - L'UML et le WIKI


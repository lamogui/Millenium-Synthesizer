
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

Signal 
------

Unité de "son", tout son continu est décomposé en signaux (un tableau 
d'échantillons), c'est ce qu'un instrument doit générer. 



Oscillator (Oscillateur)
----------
l'Oscillator est un générateur de signal périodique. Il génère un signal grâce à la méthode 
"generate". Cependant il est aussi possible de générer le signal sur un objet Signal 
exterieur grâce à la méthode "step". 




InstrumentVoice  
---------------
l'InstrumentVoice un générateur de signal (comme un oscillateur) mais il n'est lié qu'à une seule note. Le signal peut ne pas être périodique (et d'ailleurs ne doit pas l'être). 



AbstractInstrument
------------------
L'AbstractInstrument est la base d'un Instrument. Elle permet à un gestionnaire d'instrument de 
gérer les différents "Instrument" sans connaître les "InstrumentVoice" associés. 




Instrument
----------
l'Instrument est un template (un modèle), il regroupe plusieurs InstrumentVoice, et affecte chacun 
d'entre eux à une note qu'il reçoit. Il fait également le mixage final entre 
tous les signaux générés par ces InstrumentVoice. La classe Instrument 
a déjà un minimum de code fournis (gestion de la polyphonie des notes). 

N.B : polyphonie = plusieurs notes. 









Note (note.hpp)
---------------
Elle représente une note de musique : fréquence, vélocité, durée mais aussi quand 
elle a été jouée ! Quand une note est créée elle est envoyée à l'Instrument 
(via la méthode playNote(Note & n)) qui l'affectera, si il le peut, à un 
InstrumentVoice. Pour cela on utilisera la méthode beginNote(Note& n) qui elle-même préviendra 
la note via receivePlayedSignal(InstrumentVoice* v). Lorsqu'elle est terminée 
la note doit prévenir l'InstrumentVoice (méthode sendStopSignal()) afin que 
celui-ci démarre son Release Time (méthode endNote()). Ainsi, une note peut 
être terminée mais l'InstrumentVoice continue d'être "utilisé" tant qu'il n'a 
pas fini son release time.





InstrumentParameter (note.hpp)
------------------------------
Elle représente un paramètre de l'instrument codé sur un entier signé de 16 bits.
A l'initialisation l'instrument donne la plage de valeur (que le paramètre 
ne doit pas depasser). Ensuite différentes méthodes peuvent modifier cette 
valeur en utilisant une autre plage de valeurs (par exemple 
setValueFromUnsigned). InstrumentParameter sert à faire le lien( via une remise à l'échelle)  entre 
le poteniomètre et l'Instrument 







AudioStream (audiostream.hpp)
-----------------------------
Elle représente un flux, le son continu qui va sortir des haut parleurs. Il prend en entrée un objet Signal. Il s'occupe de convertir les données du signal en échantillons utilisables par le driver audio.
D'un point de vu purement technique : Il s'agit d'un buffer circulaire de type FIFO. L'atout qu'il a en plus c'est qu'il hérite d'une classe mutex il est donc verrouillable et deverrouillable (méthodes lock() et unlock()) pour éviter que 2 threads l'utilise en même temps.





AudioDriver (audiodriver.hpp)
-----------------------------
Elle représente le lien entre le Haut-Parleur et le signal converti par AudioStream lors de son démarrage (méthode start). 
On lui passe un AudioStream qu'il devra lire (via la méthode read) lorsque la 
carte son à besoin de nouveaux échantillons. 





MouseCatcher (information reçue par pa souris)
----------------------------
Elle représente un élément de l'interface graphique qui utilise la souris. 
Dans un premier temps l'utilisateur clique sur l'élément (exemple potard)
(méthode onMousePress) ce qui a pour effet "d'attraper la souris". 
Une fois qu'un MouseCatcher a attrapé la souris, il faut attendre que 
l'utilisateur relache le bouton de la souris pour en attraper un nouveau.
C'est la fonction main qui s'en charge pour le moment.
Un MouseCatcher doit être dessinable !






Knob 
--------------------
Knob veut dire potentiomètre en anglais. Cette classe est un MouseCatcher. 
Elle doit être liée à un InstrumentParameter pour agir directement dessus.





ScrollBar (barre de défilement)
-------------------------
Elle permet de gérer la vue d'une Interface si elle est trop petite pour afficher
l'integralité de l'Interface. 



Interface (interface.hpp)
-------------------------
Interface gère des sf::Drawable (choses affichables) et des MouseCatcher. Elle s'occupe de les
faire afficher dans l'espace que l'on lui donne !




Voilà j'ai fait le tour... Il faudrait faire l'UML...

D'autres TODO
=============
 - NoteTranslator : une classe qui traduit quelque chose (par exemple touches 
 de clavier) en Notes !
 - "Music" : une classe qui lit/écrit des notes dans un fichier et 
 instancie/envoie les notes aux instruments
 - Button et "Potentiomètre en ligne droite" héritant de MouseCatcher
 - InstrumentInterface : contient les potards/boutons spécifiques à un 
 instrument et envois leurs état à la classe Instrument associé
 - KeyboardInterface : affiche un clavier et envois les notes appuyés à la 
 classe Instrument associé
 - Enveloppe : Comme un oscilateur mais qui génère une enveloppe (avec attack
 decay, sustain et release)
 - L'UML
 - L'UML
 - L'UML
 - L'UML
 - L'UML
 - L'UML
 - L'UML



Petite Méga explications des classes
====================================

Signal 
------

Unité de "son", tout son continu est décomposé en signaux (un tableau d'échantillons), c'est ce qu'un instrument doit générer. (TODO : tester les constructeurs par recopie qui n'ont pas l'air de fonctionner)

Oscillator 
----------
Générateur de signal périodique il genere un signal grâce à la méthode "generate", mais il est aussi possible de generer le signal sur un objet Signal exterieur grâce à "step". (TODO: Coder les différents oscillateurs car seul le Sinusoidal est codé)

InstrumentVoice 
---------------
C'est un générateur de signal (comme un oscillateur) mais il est lié à une Note (ET UNE SEULE NOTE !), le signal peut ne pas être périodique (et d'ailleurs ne doit pas l'être). (TODO : créer un attribut instrument vers lequel l'InstrumentVoice pourra tirer ces paramètres de configuration)

Instrument 
----------
Il regroupe plusieurs InstrumentVoice, et affecte chacuns d'entre eux à une note qu'il reçoit. Il fait également  le mixage final entre tous les signaux générés par ces InstrumentVoice. (TODO : Prendre en compte des paramètres génériques (booleen/entiers/floatant), un paramètre sera identifié par un unsigned int grace à un std::map<unsigned int, type_de_pramètre>)  

Note 
----
représente une note de musique : fréquence, vélocité, durée mais aussi quand elle à été joué ! Quand une note est créée elle est envoyé à l'Instrument (via la méthode playNote(Note & n)) qui l'affectera si il le peut à un InstrumentVoice via la méthode beginNote(Note& n) qui lui même préviendra la note via receivePlayedSignal(InstrumentVoice* v). Lorsqu'elle est terminé la note doit prévenir l'InstrumentVoice (méthode sendStopSignal()) afin que celui-ci démarre son Release Time (méthode endNote()) ainsi une note peut être terminé mais l'InstrumentVoice continue d'être "utilisé" tant qu'il n'as pas fini son release time.

AudioStream 
-----------
Représente un flux, le son continu qui va sortir des haut parleurs, Il s'agit un buffer circulaire de type FIFO (c'est quasiment un copier coller du TP6 S5A d'asm), Le truc qu'il a en plus c'est qu'il est verrouillable et deverrouillable (méthodes lock() et unlock()) pour éviter que 2 threads l'utilise en même temps. Ah oui et il peut aussi ajouter directement un Signal entier. (TODO : coder la méthode bool readSignal(Signal& signal))

AudioDriver 
-----------
Représente le lien avec le Haut Parleur lors de son démarrage (méthode start) on lui passe un AudioStream qu'il devra lire (via la méthode read) lorsque la carte son à besoin de nouveaux samples. Pour l'instant il y a que 2 drivers possibles : BASS et BASSASIO (BASSASIO requiert windows et un driver asio d'installé (par exemple ASIO4ALL). (TODO mineur: Coder d'autres classes driver à l'aide d'autres librairies)

Variant 
-------
Représente quelque chose ! soit un string, soit un bool soit un nombre ! (TODO mineur: clean et optimisations)

Settings 
--------
Veille Classe Singleton codé il y a longtemps, permet de charger des paramètre dans un fichier (regarder la fonction main pour voir comment ça marche c'est tout con) (TODO mineur: clean et optimisations)


Voilà j'ai fait le tour... Il faudrait faire l'UML...

D'autres TODO
=============
 - NoteTranslator : une classe qui traduit quelque chose (par exemple touches de clavier) en Notes !
 - "Music" : une classe qui lit/écrit des notes dans un fichier et instancie/envoie les notes aux instruments
 - Boutons/Potards/Autres Conneries : pour gérer l'interface graphique 
 - AbstractInterface : une zone dans laquelle on peut dessiner et ou la souris peut fair des trucs
 - InstrumentInterface : contient les potards/boutons spécifiques à un instrument et envois leurs état à la classe Instrument associé
 - KeyboardInterface : affiche un clavier et envois les notes appuyés à la classe Instrument associé



# Simple Java JAR Launcher

[English]

A small, cross-platform launcher written in C that automatically executes a JAR (Java archive file) with the same name as the launcher's executable.

The goal is to distribute a Java application as a native executable (`.exe` on Windows or a Linux binary), without requiring the user to manually enter the command:

```bash
java -jar application.jar
```

The launcher automatically reconstructs the name of the associated `.jar` file and passes the command-line arguments to Java.

[French]

Petit lanceur multiplateforme écrit en langage C permettant d'exécuter automatiquement un fichier JAR (Java archive file) portant le même nom que l'exécutable du lanceur.

L'objectif est de pouvoir distribuer une application Java sous la forme d'un exécutable natif (`.exe` sous Windows ou binaire Linux), sans demander à l'utilisateur de saisir manuellement la commande :

```bash
java -jar application.jar
```

Le lanceur reconstruit automatiquement le nom du fichier `.jar` associé et transmet les arguments de la ligne de commande à Java.

## Fonctionnalités

* Support Windows et Linux.
* Aucun script intermédiaire nécessaire.
* Transmission des arguments de la ligne de commande.
* Utilisation des API natives du système :
	* `execvp()` sous Linux ;
	* `CreateProcess()` sous Windows.
* Pas d'utilisation de `system()`, évitant les problèmes liés au shell.
* Code source écrit en C portable.

## Fichier ressources `resources.res`

Pour l'exécutable Windows, vous pouvez ajouter des ressources à votre fichier.
Un exemple de fichier `resources.res` est inclus dans la liste des fichiers de ce projet.

Pour vous aider à créer un fichier icone, il existe des utilitaires en ligne comme celui-là :

* convertion d'un fichier PNG en fichier ICO au format Windows (16x16, 32x32, ou autes):  
[https://pngtoicon.com/](https://pngtoicon.com/ "Conversion PNG en ICO en ligne — gratuite et directe")

* IcoFX - un éditeur de fichier `*.ico` plus complet permettant toutes les conversions  
[https://www.icofx.ro/](https://www.icofx.ro/ "Full-featured icon and cursor editor for Windows")

Souvent un fichier __.ico__ de taille __32x32__ pixels, en __256 couleurs__, est suffisant.
Sa taille mémoire réduite de 2.2Ko fera un exécutable Windows compact.

Pour modifier l'exécutable et ajouter les ressources, vous pouvez utiliser l'utilitaire :

* [Resource Hacker (TM)](https://www.angusj.com/resourcehacker/ "freeware resource compiler & decompiler for Windows® applications")

Il importera le fichier `resources.res` et l'intégrera dans l'exécutable.

***

## Fonctionnement

Si l'exécutable s'appelle :

### Linux
```
mon_application
```
le lanceur recherche :

```
mon_application.jar
```

et exécute :

```bash
java -jar mon_application.jar
```

### Windows

Si l'exécutable s'appelle :

```
mon_application.exe
```

le lanceur recherche :

```
mon_application.jar
```

et exécute :

```bash
java.exe -jar mon_application.jar
```

Les arguments fournis au lanceur sont automatiquement transmis à l'application Java.

Exemple :

```bash
mon_application fichier.txt --mode=test
```

devient :

```bash
java -jar mon_application.jar fichier.txt --mode=test
```

## Compilation

__Prérequis__
* Compilateur C compatible GCC.
* Code::Blocks (optionnel).
* Java installé sur la machine cible.

__Compilation Linux__

Avec GCC :

```bash
gcc main.c -o jar_launcher
```

Exemple :

```
jar_launcher
jar_launcher.jar
```
Les deux fichiers doivent être dans le même répertoire.

__Compilation Windows__

Avec MinGW :

```bash
gcc main.c -o jar_launcher.exe
```

Exemple :

```
jar_launcher.exe
jar_launcher.jar
```

## Utilisation

__Sans argument__

Linux :

```bash
./mon_programme
```
Lance :

```bash
java -jar mon_programme.jar
```

Windows :

```cmd
mon_programme.exe
```
Lance :

```bash
java.exe -jar mon_programme.jar
```

__Avec arguments__

Exemple :

```bash
./mon_programme config.xml
```

Lance :

```bash
java -jar mon_programme.jar config.xml
```

Les espaces dans les arguments sont pris en charge.

Exemple :

```bash
./mon_programme "Mon fichier.txt"
```

devient :

```bash
java -jar mon_programme.jar "Mon fichier.txt"
```

## Organisation du code

La fonction `build_jar_name()` construit le nom du fichier JAR associé à l'exécutable.

Exemples :

```text
application.exe
```

devient :

```text
application.jar
```
### Partie Windows

Fonction spécifique Windows `jl_exec_windows()` utilisant :

```C
CreateProcess()
```
pour lancer Java.

### Partie Linux

Le lancement utilise :

```C
fork()
execvp()
waitpid()
```

Le processus fils est remplacé par la machine virtuelle Java.

### Pourquoi ne pas utiliser `system()` ?

Une première version du programme utilisait :

```C
system("java -jar application.jar");
```

Cette approche présente plusieurs inconvénients :

* dépendance au shell du système ;
* gestion complexe des guillemets ;
* risques d'injection de commandes ;
* différences de comportement entre Windows et Linux.

Le programme utilise maintenant les API natives du système pour lancer Java directement.

## Compatibilité

|__Système__			| __Méthode utilisée__|
|----------------------------|---------------------|
|Linux			| `fork()` + `execvp()`|
|Windows			| `CreateProcess()`|

Compilateurs testés :

* GCC
* MinGW
* Code::Blocks

## Limitations connues

__Java doit être accessible__

Le lanceur suppose que Java est accessible dans le `PATH`.

Vérification :

Linux :

```bash
java -version
```

Windows :

```cmd
java -version
```

__Fichier JAR obligatoire__

L'exécutable doit avoir un fichier `.jar` correspondant :

```
programme.exe
programme.jar
```

ou :

```
programme
programme.jar
```

__Arguments Windows complexes__

Les arguments contenant des caractères spéciaux Windows (`"`, `&`, `|`, etc.) peuvent nécessiter un échappement particulier.

Les cas courants avec des chemins contenant des espaces sont supportés.

## Copyright

Auteur: Eric Normandin  
Date : 2024-2026  

## Licence

Ce code source est sous licence MIT.

[French - License]

La permission est accordée par la présente, à titre gratuit, à toute personne obtenant une copie de ce logiciel et des fichiers de documentation associés (le « Logiciel »), de traiter le Logiciel sans restriction, y compris, sans limitation, les droits d’utiliser, de copier, de modifier, de fusionner, de publier, de distribuer, de concéder en sous-licence et/ou de vendre des copies du Logiciel, et de permettre aux personnes auxquelles le Logiciel est fourni de faire de même, sous réserve des conditions suivantes :

La présente mention de droit d'auteur et la présente mention d'autorisation doivent être incluses dans toutes les copies ou parties substantielles du Logiciel.

LE LOGICIEL EST FOURNI « TEL QUEL », SANS GARANTIE D'AUCUNE SORTE, EXPRESSE OU IMPLICITE, Y COMPRIS, MAIS SANS S'Y LIMITER, LES GARANTIES DE QUALITÉ MARCHANDE, D'ADÉQUATION À UN USAGE PARTICULIER ET D'ABSENCE DE CONTREFAÇON. EN AUCUN CAS, LES AUTEURS OU LES TITULAIRES DE DROITS D'AUTEUR NE SAURAIENT ÊTRE TENUS RESPONSABLES DE TOUTE RÉCLAMATION, DE TOUT DOMMAGE OU DE TOUTE AUTRE RESPONSABILITÉ, QUE CE SOIT DANS LE CADRE D'UN CONTRAT, D'UN DÉLIT OU AUTRE, DÉCOULANT DU LOGICIEL, DE SON UTILISATION OU DE TOUTE AUTRE TRANSACTION RELATIVE À CE LOGICIEL.

[English - License]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


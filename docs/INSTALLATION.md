# Installation

## Prérequis

- AzerothCore pour WotLK 3.3.5a, avec son compilateur C++20 et son installation habituelle.
- Révision de référence Sylvania : `1aea86e8932da61ba500f20f7267fcd4861cd0f5`. Les interfaces du core peuvent varier sur une autre révision.
- `EnablePlayerSettings = 1` dans `worldserver.conf`.
- Accès aux bases **auth**, **characters** et **world** du royaume concerné.

Les deux dossiers de modules sont nécessaires, même si Survival reste désactivé. Cette version de `mod-challenge-modes` appelle directement le module Survival.

## 1. Copier les modules

Clonez le dépôt hors du dossier des modules du core :

```sh
git clone https://github.com/BlaMacfly/Sylvania-Challenges.git
```

Copiez ensuite `Sylvania-Challenges/modules/mod-challenge-modes` et `Sylvania-Challenges/modules/mod-survival` dans `azerothcore/modules/`.

Pour remplacer une installation du module historique, sauvegardez son dossier puis remplacez-le par celui de cette version. Ne gardez pas une seconde copie active du même module dans `modules/`, car les scripts seraient enregistrés deux fois. Le nom des deux dossiers doit rester inchangé pour leurs chargeurs.

L'intégration Survival est **déjà présente**. L'ancien outil `prepare_integration.py` et les fichiers `.patch` sont conservés comme références de migration ; ils ne doivent pas être réappliqués aux sources de ce dépôt. Le script Lua historique `ChallengeMultiLang.lua` n'est pas nécessaire : Martyr et le menu sont en C++.

## 2. Préparer les bases

Importez ces fichiers dans la base indiquée, avec votre outil d'administration habituel :

| Base | Fichier | Rôle |
|---|---|---|
| **characters** | `modules/mod-challenge-modes/data/sql/db-characters/base/sylvania_challenge_tables.sql` | Tables de mort Hardcore et d'inscription Martyr. |
| **auth** | `modules/mod-challenge-modes/data/sql/db-auth/base/sylvania_account_vp.sql` | Colonne `account.vp` utilisée pour les Sylvania Coins. |
| **world** | `modules/mod-challenge-modes/data/sql/db-world/base/challenge_mode_gobject.sql` | Autel 254605 et apparitions dans les zones de départ. |
| **world** | `modules/mod-survival/sql/world/2026_11_07_00_survival_hunger.sql` | Aura passive de faim 900101. |

Les deux premiers fichiers créent seulement les éléments manquants et ne contiennent aucune donnée de compte ou de personnage. Le SQL de l'autel provient du module installé : il remplace l'objet 254605 et les apparitions de GUID 5530536 à 5530544. Vérifiez que ces identifiants sont disponibles dans votre base avant l'importation.

L'aura 900101 doit être libre dans les DBC et dans `spell_dbc`. Le SQL ne remplace pas une définition existante. Le module vérifie cette aura au démarrage et reste indisponible si sa définition est absente ou incorrecte. Elle est passive, n'est pas envoyée à l'interface native du client et ne nécessite aucun patch MPQ.

Le dossier `sql/world` de Survival demande cet import manuel. Pour les bases prises en charge par l'assembleur SQL d'AzerothCore, les scripts `include.sh` / `conf.sh.dist` historiques sont également conservés. Sur une installation existante, appliquez les dépendances avant de lancer le nouveau serveur.

## 3. Compiler et installer

Relancez la configuration CMake habituelle de votre core après l'ajout des deux modules, puis compilez et installez `worldserver`. Le chargeur moderne détecte les fichiers C++ et les configurations `.conf.dist` ; aucun `CMakeLists.txt` ancien n'est nécessaire dans ces modules.

Les fonctions de chargement attendues sont `Addmod_challenge_modesScripts` et `Addmod_survivalScripts`. Sur la révision de référence, les deux modules sont construits en mode statique.

## 4. Configurer

Copiez les fichiers distribués dans le dossier de configuration des modules du serveur :

```text
challenge_modes.conf.dist → challenge_modes.conf
mod_survival.conf.dist    → mod_survival.conf
```

Dans `worldserver.conf` :

```ini
EnablePlayerSettings = 1
AddonChannel = 1
```

Dans `mod_survival.conf`, pour activer le mode sur le royaume 1 :

```ini
Survival.Enabled = 1
Survival.RealmID = 1
```

`Survival.RealmID` doit correspondre au `RealmID` de votre `worldserver`. La distribution reste désactivée par défaut. Les autres réglages contrôlent les durées, les zones froides et chaudes, les feux et la pause AFK.

Le canal réservé peut être réglé dans `challenge_modes.conf` :

```ini
ChallengeModes.Channel.Enable = 1
ChallengeModes.Channel.Name = "Defis"
```

Les [profils R1](../profiles/r1) reproduisent les réglages de jeu de Sylvania. Le profil Hardcore référence l'objet 33475 pour sa récompense : la personnalisation de cet objet dans la base du serveur ne fait pas partie de ce dépôt. Martyr et les récompenses Sylvania Coins utilisent les valeurs présentes dans le code installé ; ils ne possèdent pas de nouveaux réglages inventés pour cette publication.

## 5. Installer l'addon

Décompressez `SylvaniaSurvival-0.2.0-WotLK-3.3.5a.zip` dans `Interface/AddOns/`, pour obtenir :

```text
Interface/AddOns/SylvaniaSurvival/SylvaniaSurvival.toc
Interface/AddOns/SylvaniaSurvival/SylvaniaSurvival.lua
Interface/AddOns/SylvaniaSurvival/Textures/...
```

Le pack complet contient aussi ce dossier sous `Interface/AddOns/`. Le client attendu est **3.3.5a, build 12340**. Pour une première installation, quittez complètement le jeu puis relancez-le et cochez Sylvania Survival dans la liste des addons.

Les jauges apparaissent sur un personnage Survival actif. `/survivalui test` ouvre un aperçu clairement indiqué de 60 secondes ; `stop`, `reset`, `lock`, `unlock` et `scale 80–160` règlent l'affichage. L'aperçu ne modifie aucun besoin ni aucune caractéristique.

## 6. Vérifier

Après redémarrage, le journal doit contenir :

```text
Sylvania Survival v0.2 loaded: enabled=true, hunger aura ready=true
```

La commande `.survival` affiche les réserves. Activez le défi à l'autel avec un personnage volontaire de niveau 1. Survival n'est pas proposé aux chevaliers de la mort. Le menu de la version Sylvania impose le niveau 1, contrairement au README historique qui décrit aussi les DK niveau 55.

Effectuez les essais de [VALIDATION.md](VALIDATION.md) avant une ouverture générale.

## Boutique et changements de niveau

Les protections du module contre les sauts de niveaux ne remplacent pas les contrôles d'une boutique ou d'un outil MJ. L'intégration web de Sylvania n'est pas distribuée dans ce dépôt. Le fichier `mod-survival/integration/store_api.patch` documente le contrôle Survival à ajouter à une boutique existante : refuser un boost lorsque la source `mod-survival` de `character_settings` porte le statut 1. Le test PHP associé demande explicitement le fichier de boutique concerné.

Les récompenses Sylvania Coins des défis historiques et de Martyr sont conservées. Survival n'accorde pas encore de récompense monétaire.

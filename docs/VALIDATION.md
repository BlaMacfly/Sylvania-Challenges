# Vérifications de la première publication

## Correspondance avec Sylvania R1

Les deux modules ont été exportés des sources installées sur le serveur. Les fichiers C++, configurations distribuées, SQL existants, code Lua de l'addon et textures TGA conservent leurs octets d'origine. Le manifeste [SERVER_SNAPSHOT.json](SERVER_SNAPSHOT.json) permet de le vérifier.

Les SQL des tables spécifiques, le profil de configuration, la documentation et les outils de publication complètent cette exportation. Aucun compte, personnage, journal de jeu, secret de connexion ou binaire de serveur n'est inclus.

## Contrôles déjà effectués sur R1

- Compilation de `ChallengeModes.cpp`, `Survival.cpp` et de leurs chargeurs contre la révision `1aea86e8932da61ba500f20f7267fcd4861cd0f5`.
- Compilation et lien de `worldserver`, puis démarrage avec l'aura de faim validée.
- **28 contrôles du moteur Survival**, avec AddressSanitizer et UndefinedBehaviorSanitizer.
- **27 contrôles de l'addon**, notamment les protocoles V1/V2, les malus confirmés par le serveur, les données invalides ou périmées, le changement de personnage, l'aperçu, le déplacement et la taille.
- **8 contrôles supplémentaires** sur la protection de la boutique Sylvania ; cette boutique n'est pas une dépendance distribuée du module.
- Test de déshydratation : un personnage de 60 PV a atteint 1 PV et y est resté vivant pendant 291 secondes avant sa déconnexion. Le test a été réalisé sur la version initiale du mécanisme de déshydratation, conservé dans la version 0.2.

## Vérifier ce dépôt

Sous Linux, avec Python 3.10+, un compilateur C++20 et Lua 5.1 :

```sh
python3 tools/check.py
python3 tools/package_release.py
```

Le premier outil exécute les **55 contrôles reproductibles du moteur et de l'addon**, valide les textures et vérifie les empreintes des fichiers installés. Le second crée les deux archives de release et leurs empreintes. Ces vérifications ne construisent pas un core complet.

La compilation réelle contre le core de référence fait l'objet d'un contrôle distinct avant publication. Le dossier historique `mod-survival/tools` contient aussi les outils d'intégration et de compilation utilisés sur Sylvania ; la protection de boutique y demande un fichier PHP externe.

## Essais en jeu restant à compléter

- Vérifier le rendu des trois barres et des icônes sur le client WotLK 12340, en français et en anglais.
- Vider la nourriture sans vider l'eau : contrôler les cinq caractéristiques à −50 %, puis le retrait du malus après un repas.
- Changer d'équipement et d'amélioration pendant la faim : la réduction doit rester cohérente avec le calcul natif du serveur.
- Vider l'eau sans vider la nourriture : vérifier le plafond de vie et sa levée après une boisson, sans soin gratuit.
- Vérifier un aliment, une boisson et un consommable combiné, à vie/mana pleins et après interruption.
- Comparer un personnage Survival et un personnage ordinaire dans une zone froide ; essayer une auberge, un feu de cuisine et un feu derrière un obstacle.
- Contrôler la déconnexion, la mort/résurrection, les pauses et la fin au niveau 80, ainsi que les associations avec les autres défis.

La release porte donc le statut **préversion / version de test**. Les tests d'affichage simulés et la compilation ne remplacent pas ces essais du client connecté.

# Sylvania Challenges 1.0.0-beta.1 — Martyr & Survival

Première publication de la version du module actuellement installée sur Sylvania R1, dérivée de [ZhengPeiRu21/mod-challenge-modes](https://github.com/ZhengPeiRu21/mod-challenge-modes).

Les huit défis historiques sont conservés et accompagnés de nos deux défis :

- **Martyr** : perte d'XP à la mort, possibilité de perdre un niveau et récompense au niveau 80.
- **Survival** : nourriture, hydratation et froid. Faim à zéro : −50 % aux cinq caractéristiques principales. Hydratation à zéro : vie abaissée progressivement jusqu'à 1 %. Les abris et feux de cuisine permettent de se réchauffer.

Cette version comprend le canal réservé `Defis`, les annonces, les récompenses Sylvania, les configurations, les SQL et le guide d'installation.

## Fichiers à télécharger

- **Sylvania-Challenges-v1.0.0-beta.1.zip** : pack complet des deux modules et de l'addon Survival. Les sources sont dans `modules/` ; l'addon prêt à copier se trouve aussi dans `Interface/AddOns/SylvaniaSurvival/`.
- **SylvaniaSurvival-0.2.0-WotLK-3.3.5a.zip** : addon seul pour le client WotLK 12340, avec les trois barres, les icônes originales et l'affichage des malus.
- **SHA256SUMS.txt** : empreintes des deux archives.

## Installation et validation

Installez **mod-challenge-modes et mod-survival ensemble**. L'intégration est déjà présente : ne réappliquez pas les anciens patchs. Le guide du dépôt détaille les dépendances des bases et la configuration.

Les sources correspondent à celles exportées du serveur ; leurs empreintes sont publiées. Le moteur Survival et l'addon disposent de **55 contrôles automatisés**. La compilation avec le core R1 a déjà été vérifiée.

**Fonctionnalité à tester :** cette release est une préversion. Le rendu du client, la réduction des caractéristiques par la faim et la récupération après consommation doivent encore être validés en jeu. `/survivalui test` ouvre un aperçu sans modifier le personnage.

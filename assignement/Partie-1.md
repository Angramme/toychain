# Partie 1
Dans cette première partie nous nous intéressons au chiffrement et dechiffrement d'un message à l'aide du protocole RSA.
Dans le premier exercice on implémente des fonctions permettant de calculer des nombres premiers dont le test 
de Miller-Rabin.
Dans l'exercice suivant, on utilise les fonctions précédentes pour déterminer des clés publiques et privées grâce
à l'algorithme d'Euclide.

## Exos 1 et 2 du Sujet

### Q1.1
Dans le cas ou p est premier, c'est-a-dire le pire cas, la complexité de is_prime_naive est O(p)

### Q1.3
La complexité de modpow_naive est O(m)

### Q1.5
Pour les 2 fonctions, les temps sont très faibles même pour des grands m.
On remarque cependant qu'au bout d'un moment le durée de modpow_naive commence à augmenter.
Pour modpow, la durée est trop courte, on ne remarque pas d'augmentation même pour des grands nombres.
Exemple :
    Pour a=9435000329, m=134217728, n=1093
    durée modpow_naive = 1.491 sec
    durée modpow ~ 0 sec
### Q1.7
Parmi les valeurs de 2 a p-1, cad p-3 valeurs. On a une probabilité de 1/4 de ne pas tomber sur un temoin de Miller.
De plus on effectue le test k fois avec remise (la fonction rand_long peut tomber deux fois sur la meme valeur).
Autrement dit la probabilite ne change pas entre les lancers. Donc la probabilite de ne pas tomber sur un temoin de Miller 
a k test consecutifs est de (1/4)^k.

> Rmq: Il serait tres preferable de adapter notre fonction de tirage aleatoire pour qu'elle ne repete pas 
> deux fois la meme valeur, cela augmenterait la chance de tomber sur un temoin de Miller a chaque iteration consecutive et 
> par consequent diminuerait de faire un faux positif pour un meme nombre des tests k.

### Jeu de tests
Les détails des sets utilisés pour les tests sont disponibles dans ./test
Pour lancer les tests : make test
# Partie 1
Dans cette première partie nous nous intéressons au chiffrement et dechiffrement d'un message à l'aide du protocole RSA.
Dans le premier exercice on implémente des fonctions permettant de calculer des nombres premiers dont le test 
de Miller-Rabin.
Dans l'exercice suivant, on utilise les fonctions précédentes pour déterminer des clés publiques et privées grâce
à l'algorithme d'Euclide.

## Exos 1 et 2 du Sujet

### Q1.1
Dans le cas où p est premier, c'est-à-dire le pire cas, la complexité de is_prime_naive est O(p).

### Q1.3
La complexité de modpow_naive est O(m).

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
Autrement dit la probabilité ne change pas entre les lancers. Donc la probabilité de ne pas tomber sur un temoin de Miller 
pour k tests consecutifs est de (1/4)^k.

> Rmq: Il serait tres preferable d'adapter notre fonction de tirage aléatoire pour qu'elle ne répète pas 
> deux fois la meme valeur. Cela augmenterait la probabilité de tomber sur un temoin de Miller à chaque iteration consecutive et 
> par consequent diminuerait celle de faire un faux positif pour un même nombre des tests k.

### Jeu de tests
Les sets utilisés pour les tests de chiffrement sont générés aléatoirement. On implémente dans ./test/test.h des fonctions
qui permettent de faires des tests simplements (voir la doc).

> Difficultés rencontrées : 
> Nous nous sommes aperçus que l'implémentation de l'algorithme d'Euclide donnée dans le sujet comportait des failles.
> En effet, l'algorithme n'empêche pas les overflows, ce qui peut pérturber la génération de clés publiques, et donc fausser
> le chiffrage. Pour parer à cela nous avons introduit des fonctions qui vérifient, pour une opération donnée, si elle cause
> un overflow. Ensuite pour générer une clé, si il y a overflow, on recommence la génération tant que c'est toujours le cas.
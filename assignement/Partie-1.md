# Partie 1
Dans cette première partie nous nous intéressons au chiffrement et dechiffrement d'un message à l'aide du protocole RSA.
Dans le premier exercice on implémente des fonctions permettant de calculer des nombres premiers dont le test 
de Miller-Rabin.
Dans l'exercice suivant, on utilise les fonctions précédentes pour déterminer des clés publiques et privées grâce
à l'algorithme d'Euclide.

## Exos 1 et 2 du Sujet

### Q1.1
Dans le cas où p est premier, c'est-à-dire le pire cas, la complexité de is_prime_naive est O(p).

### Q1.2
TODO
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
Les sets utilisés pour les tests de chiffrement sont générés aléatoirement. On implémente dans test/test.h des fonctions
qui permettent de faires des tests simplements ainsi que lib/overflow.h pour detecter des overflows (voir la doc).

## Difficultés rencontrées
Nous nous sommes rendus compte, dans la fonction __generate_key_values()__, que certaines valeurs générées pouvaient causer
des overflows et ainsi perturber le chiffrement/dechiffrement.

C'est le cas de l'argument n de cette même fonction. En effet, si n est trop grand, sa multiplication par lui-même peut causer un overflow.
Et il en va de même pour le produit de certains nombres modulo n.
Par exemple :  x = (a % n) * (b % n) peut depasser la taille d'un entier.

Le problème vient du fait que n est ensuite utilisé pour l'exponentiation modulaire __modpow()__ et peut donc induire un mauvais chiffrement ou dechiffrement. Effectivement, si n a 'débordé', l'opération modulo n donnera
un résultat non valide pour notre utilisation. Or n est calculé à partir du produit de p et q. Donc si p,q sont trop grands,
p*q peut causer un overflow.

Pour empêcher cela, nous avons : 
1. Créé des runtime warnings, des fonctions qui vérifient si une opération entre deux entiers cause un overflow (voir overflow.h).
2. Généré un nouveau couple (p,q) d'entiers premiers tant qu'un overflow a eu lieu pendant la génération, ie tant que n = p*q est trop grand.

Ainsi, on s'assure que le couple (p,q) ne cause pas d'overflow et qu'il permet bien de chiffrer/dechiffrer le message.
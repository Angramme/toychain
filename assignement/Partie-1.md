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

> Difficultés rencontrées : 
> On s'apercu que pour une valeur de modulo trop grande, les résultats de la fonction modpow sont faussées a cause des overflows.
> En effet si n*n depasse la taille de entier, alors il en va de meme pour certains nombres a, b et leur produit modulo n i.e 
> x = (a%n) * (b%n) peut depasser la taille d'un entier.
> Par consequent si notre paire des clès generes comporte un n où n*n depasse la taille de l'entier alors notre encryption et 
> decryption donnent des faux resultats.
> Pour résoudre ces problemes on a commencé par introduire des runtime warnings, pour detecter quand un overflow 
> s'est produit dans la fonction modpow (a l'aide des defines dans lib/overflow.h). 
> Notre théorie a été bien confirmé, dans les test-cases où le résultat etaient faux
> on avait bien des overflows dans modpow et dans celles qui passait on en avait pas.
> Par consequent on a decidé de ajouter un check a la fonction generate_key_values : si n*n fait un overflow alors il est 
> impossible de génerer une paire des clé valide avec p et q passés en parametre. 
> Cela nous conduit a essayer plusieurs paires des nombres premiers aleatoires avant que on trouve une où n n'est pas trop grand.
> Cette approche a eliminé tout les overflows dans notre programme.

# Compilation

1. Lancer `cargo build --release`
2. L'exécutable est `./target/release/client`

# Lancement

Lancer l'exécutable précédemment compilé, avec une configuration en argument.  
Exemple : `./target/release/client affichage1.cfg`

Afin d'obtenir un aquarium complet, avec toutes les vues, simplement lancer plusieurs aquariums avec les bonnes configurations.  
Exemple : avec l'aquarium par défaut, lancer 4 vues avec `affichage1.cfg`, `affichage2.cfg`, `affichage3.cfg` et `affichage4.cfg`.

# Développement

Lancer le projet via `cargo run -- <fichier de configuration>`  
Exemple : `cargo run -- <affichage1.cfg>`
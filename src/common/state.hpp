#pragma once

// Enum pour les boutons
enum class ButtonKey {
    Login,          // Bouton pour se connecter
    Registre,       // Bouton pour s'inscrire
    Quit,           // Bouton pour quitter
    Teams,          // Bouton pour gérer les équipes
    Play,           // Bouton pour jouer
    Chat,           // Bouton pour accéder au chat
    Send,           // Bouton pour envoyer un message
    Contacts,       // Bouton pour afficher les contacts
    Friends,        // Bouton pour gérer les amis
    Settings,       // Bouton pour accéder aux paramètres
    Notification,   // Bouton pour afficher les notifications
    Profile,        // Bouton pour afficher le profil
    Create,         // Bouton pour créer une partie
    Join,           // Bouton pour rejoindre une partie
    Endless,        // Bouton pour le mode sans fin
    Duel,           // Bouton pour le mode duel
    Classic,        // Bouton pour le mode classique
    Royale,         // Bouton pour le mode royale
    Rejouer,        // Bouton pour rejouer
    Retour          // Bouton pour revenir en arrière
};

// Enum pour les champs de texte
enum class TextFieldKey {
    Username,       // Champ de texte pour le nom d'utilisateur
    Password,       // Champ de texte pour le mot de passe
    MessageField,   // Champ de texte pour les messages
    SearchField     // Champ de texte pour la recherche
};
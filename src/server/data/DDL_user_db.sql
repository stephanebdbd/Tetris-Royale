CREATE TABLE IF NOT EXISTS Users (
    id_user INTEGER PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    hash_pwd TEXT NOT NULL,
    salt TEXT NOT NULL,
    CHECK(username <> '')
);

CREATE TABLE IF NOT EXISTS ChatRooms (
    id_room INTEGER PRIMARY KEY AUTOINCREMENT,
    room_name TEXT UNIQUE NOT NULL,
    room_properietors INTEGER NOT NULL,
    FOREIGN KEY (room_properietors) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    CHECK(room_name <> ''),
    CHECK(room_properietors <> '')
);

CREATE TABLE IF NOT EXISTS ChatRoomMembers (
    id_room INTEGER NOT NULL,
    id_user INTEGER NOT NULL,
    status TEXT NOT NULL DEFAULT 'pending' CHECK (status IN ('pending', 'member', 'admin')),
    PRIMARY KEY (id_room, id_user),
    FOREIGN KEY (id_room) REFERENCES ChatRooms(id_room) ON DELETE CASCADE,
    FOREIGN KEY (id_user) REFERENCES Users(id_user) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS ChatMessages (
    id_message INTEGER PRIMARY KEY AUTOINCREMENT,
    id_room INTEGER NOT NULL,
    sender TEXT NOT NULL,
    message TEXT NOT NULL,
    msg_date_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_room) REFERENCES ChatRooms(id_room) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Messages (
    id_msg INTEGER PRIMARY KEY AUTOINCREMENT,
    sender TEXT NOT NULL,
    receiver TEXT NOT NULL,
    msg TEXT NOT NULL,
    msg_date_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (sender) REFERENCES Users(username)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    FOREIGN KEY (receiver) REFERENCES Users(username)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    CHECK (sender <> receiver), -- Un utilisateur ne peut pas envoyer de message à lui-même
    CHECK(msg <> '') -- Vérifier que le message n'est pas vide
);

CREATE TABLE IF NOT EXISTS Friendships (
    id_friendship INTEGER PRIMARY KEY,
    id_sender INTEGER NOT NULL,
    id_receiver INTEGER NOT NULL,
    status TEXT NOT NULL CHECK (status IN ('pending', 'accepted')),
    UNIQUE (id_sender, id_receiver),
    FOREIGN KEY (id_sender) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    FOREIGN KEY (id_receiver) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    CHECK (id_sender <> id_receiver)
);
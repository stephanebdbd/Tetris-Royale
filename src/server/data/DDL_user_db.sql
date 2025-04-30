CREATE TABLE IF NOT EXISTS Users (
    id_user INTEGER PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    passwrd TEXT NOT NULL,
    best_score INTEGER DEFAULT 0,
    id_avatar TEXT NOT NULL DEFAULT -1,
    CHECK(username <> '')
);

CREATE TABLE IF NOT EXISTS Games(
    id_game INTEGER,
    id_sender INTEGER,
    id_player INTEGER,
    invitation_type TEXT NOT NULL CHECK (invitation_type IN ('player', 'observer')),
    status TEXT NOT NULL DEFAULT 'pending' CHECK (status IN ('pending', 'accepted')),
    PRIMARY KEY (id_game, id_player),
    FOREIGN KEY (id_sender) REFERENCES Users(id_user),
    FOREIGN KEY (id_player) REFERENCES Users(id_user)
);

CREATE TABLE IF NOT EXISTS ChatRooms (
    id_room INTEGER PRIMARY KEY AUTOINCREMENT,
    room_name TEXT UNIQUE NOT NULL,
    room_proprietor INTEGER NOT NULL,
    FOREIGN KEY (room_proprietor) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    CHECK(room_name <> '')
);

-- 'invitation' when an admin sends an invitation to client
-- 'pending' when a client sends a request to join a room
CREATE TABLE IF NOT EXISTS ChatRoomMembers (
    id_room INTEGER NOT NULL,
    id_user INTEGER NOT NULL,
    status TEXT NOT NULL DEFAULT 'pending' CHECK (status IN ('invitation', 'pending', 'member', 'admin')),
    PRIMARY KEY (id_room, id_user),
    FOREIGN KEY (id_room) REFERENCES ChatRooms(id_room) ON DELETE CASCADE,
    FOREIGN KEY (id_user) REFERENCES Users(id_user) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS ChatMessages (
    id_message INTEGER PRIMARY KEY AUTOINCREMENT,
    id_room INTEGER NOT NULL,
    sender INTEGER NOT NULL,
    message TEXT NOT NULL,
    msg_date_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_room) REFERENCES ChatRooms(id_room) ON DELETE CASCADE,
    FOREIGN KEY (sender) REFERENCES Users(id_user)
);

CREATE TABLE IF NOT EXISTS Messages (
    id_msg INTEGER PRIMARY KEY AUTOINCREMENT,
    sender INTEGER NOT NULL,
    receiver INTEGER NOT NULL,
    msg TEXT NOT NULL,
    msg_date_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (sender) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    FOREIGN KEY (receiver) REFERENCES Users(id_user)
        ON DELETE CASCADE
        ON UPDATE NO ACTION,
    CHECK (sender <> receiver), -- A user cannot send a message to themselves
    CHECK(msg <> '') -- Check that the message is not empty
);

CREATE TABLE IF NOT EXISTS Friendships (
    id_friendship INTEGER PRIMARY KEY AUTOINCREMENT,
    id_sender INTEGER NOT NULL,
    id_receiver INTEGER NOT NULL,
    status TEXT NOT NULL CHECK (status IN ('pending', 'accepted', "bestFriend", 'banned')),
    UNIQUE (id_sender, id_receiver),
    FOREIGN KEY (id_sender) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    FOREIGN KEY (id_receiver) REFERENCES Users(id_user)
        ON DELETE CASCADE 
        ON UPDATE NO ACTION,
    CHECK (id_sender <> id_receiver)
);
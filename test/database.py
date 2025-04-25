import sqlite3

def display_database_contents(db_path):
    try:
        # Connexion à la base de données
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Récupérer la liste des tables
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
        tables = cursor.fetchall()

        if not tables:
            print("Aucune table trouvée dans la base de données.")
            return

        # Parcourir chaque table et afficher son contenu
        for table_name in tables:
            table_name = table_name[0]
            print(f"\nContenu de la table '{table_name}':")
            
            # Récupérer les colonnes de la table
            cursor.execute(f"PRAGMA table_info({table_name});")
            columns = [col[1] for col in cursor.fetchall()]
            print(f"Colonnes: {', '.join(columns)}")

            # Récupérer les données de la table
            cursor.execute(f"SELECT * FROM {table_name};")
            rows = cursor.fetchall()

            if rows:
                for row in rows:
                    print(row)
            else:
                print("La table est vide.")

    except sqlite3.Error as e:
        print(f"Erreur lors de l'accès à la base de données : {e}")
    finally:
        if conn:
            conn.close()

# Chemin vers la base de données
db_path = "../src/server/data/database.db"
display_database_contents(db_path)

#command for test
#python3 database.py
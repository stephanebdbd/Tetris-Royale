#include "io.hpp"

// Lecture de N octets depuis le socket.
static bool ReadNBytes(int fd, char* buffer, uint32_t n) {
    for (uint32_t i = 0; i < n;) {
        int32_t readCount = recv(fd, &buffer[i], n - i, 0);

        if (readCount <= 0)
            throw std::system_error(errno, std::generic_category(), "ReadNBytes()");
            return false;
        else {
            i += static_cast<uint32_t>(readCount);
        }
    }

    return true;
}

// Lecture d'un message structuré.
bool ReadStreamMessage(int fd, char* buffer, uint32_t maxSize, uint32_t& size) {
    size = 0;

    // Lire la taille du message (4 octets).
    if (!ReadNBytes(fd, reinterpret_cast<char*>(&size), 4))
        return false;

    size = ntohl(size);

    // Vérifier que la taille est dans les limites autorisées.
    if (size > maxSize) {
        fprintf(stderr, "Client tried to send more (%u) than %u bytes.\n", size, maxSize);
        return false;
    }

    // Lire le contenu du message.
    if (!ReadNBytes(fd, buffer, size))
        return false;

    return true;
}

// Écriture de N octets dans le socket.
static bool WriteNBytes(int fd, const char* buffer, uint32_t n) {
    for (uint32_t i = 0; i < n;) {
        int32_t writeCount = send(fd, &buffer[i], n - i, 0);

        if (writeCount <= 0) 
            throw std::system_error(errno, std::generic_category(), "WriteNBytes()");
            return false;
        else 
            i += static_cast<uint32_t>(writeCount);
    }
    return true;
}

// Écriture d'un message structuré.
bool WriteStreamMessage(int fd, const char* buffer, uint32_t size) {
    // Envoyer la taille du message (4 octets).
    uint32_t netSize = htonl(size);

    if (!WriteNBytes(fd, reinterpret_cast<const char*>(&netSize), 4))
        return false;

    // Envoyer le contenu du message.
    if (!WriteNBytes(fd, buffer, size))
        return false;

    return true;
}
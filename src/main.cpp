#include "webserv.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
    try {
        // Vérification des arguments
        if (argc != 2) {
            std::cerr << RED << "Usage: " << argv[0] << " <config_file>" << RESET << std::endl;
            return 1;
        }

        std::cout << BLUE << "🚀 Démarrage de Webserv..." << RESET << std::endl;
        
        // Parsing de la configuration
        std::cout << YELLOW << "📖 Lecture du fichier de configuration: " << argv[1] << RESET << std::endl;
        ConfigParser config(argv[1]);
        
        // Initialisation du serveur
        std::cout << GREEN << "⚙️  Initialisation du serveur..." << RESET << std::endl;
        Server server(config);
        
        // Démarrage du serveur
        std::cout << GREEN << "🌐 Serveur démarré avec succès!" << RESET << std::endl;
        server.run();
        
    } catch (const std::exception& e) {
        std::cerr << RED << "❌ Erreur: " << e.what() << RESET << std::endl;
        return 1;
    }
    
    return 0;
}

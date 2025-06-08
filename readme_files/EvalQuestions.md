**Mandatory Part**
_Launch the installation of siege with homebrew._
...

_Ask explanations about the basics of an HTTP server._
Un serveur HTTP est un programme qui écoute les requêtes des navigateurs (ou d'autres clients) sur un port spécifique, puis leur envoie les ressources web demandées (pages HTML, images, etc.) accompagnées d'un code de statut. C'est le "livreur" du web, attendant des commandes et y répondant.

_Ask what function the group used for I/O Multiplexing._
Select()

_Ask for an explanation of how does select() (or equivalent) work._
select() surveille les descripteurs de fichier jusqu'à \_max_fd pour voir s'ils sont prêts à lire (\_read_fds) ou à écrire (\_write_fds), ou si le délai (timeout) est écoulé. La fonction retourne le nombre de descripteurs prêts, ou -1 en cas d'erreur.

_Ask if they use only one select() (or equivalent) and how they've managed the server to accept and the client to read/write._
D'après le code dans Server.cpp, nous utilisons un seul select() dans la boucle principale du serveur, qui gère à la fois l'acceptation des nouvelles connexions (handleNewConnection()) et la lecture/écriture des clients existants (handleClientRequest()) en utilisant deux ensembles de descripteurs de fichiers : \_read_fds et \_write_fds.

_The select() (or equivalent) should be in the main loop and should check file descriptors for read and write AT THE SAME TIME. If not, the grade is 0 and the evaluation process ends now._
Le code passe ce critère d'évaluation car il utilise select() pour surveiller les opérations de lecture ET d'écriture en même temps dans sa boucle principale.

_There should be only one read or one write per client per select() (or equivalent). Ask the group to show you the code from the select() (or equivalent) to the read and write of a client_
Le code respecte la règle car il n'effectue qu'une seule lecture (recv()) ou une seule écriture (send()) par client après chaque appel à select(), évitant ainsi les lectures/écritures multiples.

_Search for all read/recv/write/send on a socket and check that, if an error is returned, the client is removed._
Juste à check tous les read/recv/write/send dans le code.

_Search for all read/recv/write/send and check if the returned value is correctly checked (checking only -1 or 0 values is not enough, both should be checked)._
Juste à check tous les read/recv/write/send dans le code.

_If errno is checked after read/recv/write/send, the grade is 0 and the evaluation process ends now._
Juste à check tous les read/recv/write/send dans le code.

_Writing or reading ANY file descriptor without going through the select() (or equivalent) is strictly FORBIDDEN._
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

_The project must compile without any re-link issue. If not, use the 'Invalid compilation' flag._
Bahhhh compile et recomplile zebi

_If any point is unclear or is not correct, the evaluation stops._
Rompich

**Configuration**

_Search for the HTTP response status codes list on the internet. During this evaluation, if any status codes is wrong, don't give any related points._
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

_Setup multiple servers with different ports._
/WebServ/configs/multi_server_test.conf

_Setup multiple servers with different hostnames (use something like: curl --resolve example.com:80:127.0.0.1 http://example.com/)._
/WebServ/configs/default.conf

_Setup default error page (try to change the error 404)._
Montrer la page error 404 ??????????????????????????????????

_Limit the client body (use: curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit")._
À test à 19 i guess

_Setup routes in a server to different directories._
À test à 19 i guess

_Setup a default file to search for if you ask for a directory._
À test à 19 i guess

_Setup a list of methods accepted for a certain route (e.g., try to delete something with and without permission)._
À test à 19 i guess

**Basic checks**

_GET, POST and DELETE requests should work._
GET : curl -X GET http://localhost:8080/
POST : curl -X POST -H "Content-Type: plain/text" --data "Test data" http://localhost:8080/upload
DELETE : curl -X DELETE http://localhost:8080/file-to-delete.txt

_UNKNOWN requests should not result in a crash._
UNKNOWN : curl -X UNKNOWN http://localhost:8080/ (devrait retourner 405 Method Not Allowed)

_For every test you should receive the appropriate status code._
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

_Upload some file to the server and get it back._
Test d'upload/download

**Check CGI**

_The server is working fine using a CGI._
Montrer les fonctions du CGI dans server.cpp i guess

_The CGI should be run in the correct directory for relative path file access._
Euh ouais ça doit etre ça ouais !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

_With the help of the students you should check that everything is working properly. You have to test the CGI with the "GET" and "POST" methods._
Gnnnnnnnnn !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

_You need to test with files containing errors to see if the error handling works properly. You can use a script containing an infinite loop or an error; you are free to do whatever tests you want within the limits of acceptability that remain at your discretion. The group being evaluated should help you with this._
Gnnnnnnnnn !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

**Check with a browser**
_Use the reference browser of the team. Open the network part of it, and try to connect to the server using it._
ui

_Look at the request header and response header._
inspect

_It should be compatible to serve a fully static website._
ui

_Try a wrong URL on the server._
Try à 19

_Try to list a directory._
Try à 19

_Try a redirected URL._
Try à 19

_Try anything you want to._
Try à 19

**Port issues**

_In the configuration file setup multiple ports and use different websites. Use the browser to ensure that the configuration works as expected and shows the right website._
Ouvrir les 2 serv en meme temps (8080 et 8081)

_In the configuration, try to setup the same port multiple times. It should not work._
PAS SUR QU'ON LE CHECK CA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

_Launch multiple servers at the same time with different configurations but with common ports. Does it work? If it does, ask why the server should work if one of the configurations isn't functional. Keep going._
Try à 19

**Siege & stress test**

_Use Siege to run some stress tests._
Test simple (10 utilisateurs concurrents pendant 30 secondes) : siege -c10 -t30S http://localhost:8080/
Test intensif (50 utilisateurs concurrents pendant 1 minute) : siege -c50 -t1M http://localhost:8080/

_Availability should be above 99.5% for a simple GET on an empty page with a siege -b on that page._
Try à 19

_Verify there is no memory leak (Monitor the process memory usage. It should not go up indefinitely)._
Try à 19

_Check if there is no hanging connection._
Try à 19

_You should be able to use siege indefinitely without having to restart the server (take a look at siege -b)._
Try à 19

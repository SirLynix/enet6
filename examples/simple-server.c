#include <enet6/enet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    ENetAddress address;
    ENetHost* serverHost;
    ENetEvent event;
    int eventStatus;
    char addressBuffer[ENET_ADDRESS_MAX_LENGTH];

    /* Initialize enet6 */
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occured while initializing ENet6.\n");
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);

    /* Build the listen address (any + port) */
    enet_address_build_any(&address, ENET_ADDRESS_TYPE_IPV6);
    address.port = 1234;

    /* Create a host using enet_host_create, address type has to match the address,  */
    /* except for the combination IPv6 + Any which enables dual stack (IPv6 socket allowing IPv4 connection)  */
    serverHost = enet_host_create(ENET_ADDRESS_TYPE_ANY, &address, 32, 2, 0, 0);
    if (serverHost == NULL)
    {
        fprintf(stderr, "An error occured while trying to create an ENet6 server host\n");
        exit(EXIT_FAILURE);
    }

    /* Connect and user service */
    eventStatus = 1;

    while (1)
    {
        eventStatus = enet_host_service(serverHost, &event, 100);

        /* If we had some event that interested us */
        if (eventStatus > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    if (enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH) != 0)
                        strcpy(addressBuffer, "<unknown>");
                    printf("(Server) We got a new connection from %s\n", addressBuffer);
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                    printf("(Server) Message from client : %s\n", event.packet->data);
                    /* Re-send the message to all clients */
                    enet_host_broadcast(serverHost, 0, event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                {
                    if (enet_address_get_host_ip(&event.peer->address, addressBuffer, ENET_ADDRESS_MAX_LENGTH) != 0)
                        strcpy(addressBuffer, "<unknown>");
                    printf("Client %s disconnected%s.\n", addressBuffer, (event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT) ? " (timeout)" : "");
                    break;
                }
            }
        }
    }
}

#include <enet6/enet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    ENetAddress address;
    ENetHost* clientHost;
    ENetPeer* serverPeer;
    ENetEvent event;
    int eventStatus;
    char message[1024];
    char addressBuffer[ENET_ADDRESS_MAX_LENGTH];
    int running = 1;

    /* Initialize enet6 */
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occured while initializing ENet6.\n");
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);

    /* Build an IPv6 or IPv4 address, depending on what the domain resolves to */
    enet_address_set_host(&address, ENET_ADDRESS_TYPE_ANY, "localhost");
    address.port = 1234;

    if (enet_address_get_host_ip(&address, addressBuffer, ENET_ADDRESS_MAX_LENGTH) != 0)
        strcpy(addressBuffer, "<unknown>");
    printf("Connecting to %s...\n", addressBuffer);

    /* Create a non-listening host using enet_host_create */
    /* Note we create a host using the same address family we resolved earlier */
    clientHost = enet_host_create(address.type, NULL, 1, 2, 0, 0);
    if (clientHost == NULL)
    {
        fprintf(stderr, "An error occured while trying to create an ENet6 server host\n");
        exit(EXIT_FAILURE);
    }

    /* Connect and user service */
    serverPeer = enet_host_connect(clientHost, &address, 2, 0);
    if (serverPeer == NULL)
    {
        fprintf(stderr, "No available peers for initializing an ENet connection");
        exit(EXIT_FAILURE);
    }

    while (running)
    {
        eventStatus = enet_host_service(clientHost, &event, 100);

        /* Inspect events */
        if (eventStatus > 0)
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("(Client) Connected to server %s\n", addressBuffer);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    printf("(Client) Message from server: %s\n", event.packet->data);
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("(Client) Disconnected from server.\n");
                    return EXIT_SUCCESS;

                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                    printf("(Client) Disconnected from server (timed out).\n");
                    return EXIT_FAILURE;
            }
        }
        else if (serverPeer->state == ENET_PEER_STATE_CONNECTED)
        {
            /* Prompt some message to send to the server, be quick to prevent timeout (TODO: Read asynchronously) */
            printf("> ");
            fgets(message, sizeof(message), stdin);

            if (strlen(message) > 0 && strcmp(message, "\n") != 0)
            {
                /* Build a packet passing our bytes, length and flags (reliable means this packet will be resent if lost) */
                ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
                /* Send the packet to the server on channel 0 */
                enet_peer_send(serverPeer, 0, packet);
            }
            else
            {
                running = 0;
                enet_peer_disconnect_now(serverPeer, 0);
            }
        }
    }
}

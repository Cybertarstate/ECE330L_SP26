package HW1;

import java.io.EOFException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.Date;
import java.net.*;




public class PingPongServer {

    private long clientCount= 1;
    private int port = 0;

    public PingPongServer(int port) {
        this.port = port;
    }



	/**
	 * The main method for the server -- runs forever or until the client closes
	 * the connection.
	 * 
	 * @throws IOException
	 */
    public void runServer() throws IOException
    {
        @SuppressWarnings("resource")
        ServerSocket serverSocket = new ServerSocket(port);
        System.out.println("Server:ready");
        while(true){
            new ServerConnection(serverSocket.accept(), clientCount).start();
            System.out.println("Server: Started client #" + clientCount );
            clientCount++;
        }
    }

    private class ServerConnection extends Thread {
        private Socket client;
        private long clientCount;

        ServerConnection(Socket client, long requestNum) throws SocketException {
            this.clientCount = requestNum;
            this.client = client;
            setPriority(NORM_PRIORITY - 1);
        }


    

        public void run()
        {
            try {
                ObjectOutputStream out = new ObjectOutputStream(client.getOutputStream());
                ObjectInputStream in = new ObjectInputStream(client.getInputStream());
                System.out.println("connected" + client.getInetAddress().getHostAddress());
        
                while(true) {
                    out.writeObject(processRequest(in.readObject()));
                    out.flush();
                }
        
            } catch (EOFException e) {// Normal EOF
                    try {
                        client.close();
                        System.out.println("Server Connection finished " + clientCount);

                    } catch (IOException e1) {
                        System.err.println(e1);
                    }            
            } catch(IOException e) {
                    System.err.println("I/O errer" + e); //IO err
            } catch(ClassNotFoundException e) {
                System.err.println(e);//Unknown request
            }
        }

        private Object processRequest(Object request)
        {
            if (request instanceof DateRequest)
                return new java.util.Date();
            else if(request instanceof WorkRequest)
                return((WorkRequest) request).execute();
            else
                return null;
        }

    }

    public static void main(String[] args) throws IOException
    {
        if (args.length < 1){
            System.err.println("Usage: Java PingPongServer <port>");
            System.exit(1);
        }
        int port = Integer.parseInt(args[0]);
        PingPongServer pongServer = new PingPongServer(port);  
        pongServer.runServer();
    
    }
}

    


package HW1;
import java.io.InputStream;
import java.io.ObjectInput;
import java.io.ObjectInputStream;
import java.net.*;
import java.util.Date;
import java.io.IOException;
import java.net.Socket;

public class Client 
{
    private String host;
    private int port;
    
    public Client(String host, int port)
    {
        this.host = host;
        this.port = port;
    }


    public void connect()
    {
        try {
        Socket socketed = new Socket(host,port);
        InputStream in = socketed.getInputStream();
        System.out.println("We connected at port" + port 
                + "with the address" + socketed.getLocalAddress()+"to host" + host);

        socketed.close();
        } catch(IOException e) {
            //sys err no connect

        }
    }

    public static void main(String[] args)
    {
        if (args.length !=2) {
            System.err.println("Usage: java Client <server-host> <port>");
            System.exit(1);
        }
        String host = args[0];
        int port = Integer.parseInt(args[1]);

        Client pingpongClient = new Client(host,port);
        pingpongClient.connect();


    }




}
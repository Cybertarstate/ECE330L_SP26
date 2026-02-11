package HW1;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

public class Client 
{

    public static void main(String[] args)
    {
        if (args.length < 2) {
            System.err.println("Usage: java Client <server-host> <port>");
            System.exit(1);
        }
        try {
        Socket serverhost = new Socket(args[0],Integer.parseInt(args[1]));
        ObjectOutputStream out = new ObjectOutputStream(serverhost.getOutputStream());
        ObjectInputStream in = new ObjectInputStream(serverhost.getInputStream());
        System.out.println("We connected at port" + args[1] 
                + "with the address" + serverhost.getLocalAddress()+"to host" + args[0]);

        out.writeObject(new DateRequest());
        out.flush();
        System.out.println(in.readObject());
        serverhost.close();
        } catch (IOException e) {
            System.out.println("I/O error " + e);
        } catch (ClassNotFoundException e2) {
            System.out.println(e2);
        }

    }

}
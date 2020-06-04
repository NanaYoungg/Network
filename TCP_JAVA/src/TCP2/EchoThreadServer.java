package TCP2;

import java.net.ServerSocket;
import java.net.Socket;

public class EchoThreadServer {

	public static void main(String[] args) {
		// TODO Auto-generated method stub

		try {
			ServerSocket server = new ServerSocket(900);
			System.out.println("------------접속 대기 ------------");
			while (true){
				Socket sock = server.accept();
				EchoThread ehcothread = new EchoThread(sock);
				ehcothread.start();
				
			}
		}catch(Exception e) {
			e.printStackTrace();
		}
		
		
	}

}

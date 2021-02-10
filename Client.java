import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Scanner;

public class Client {

	public static boolean isValidName(String name) {
		if(name.isBlank()|| name.isEmpty())
			return false;
		
		boolean isValid1 = false;
		boolean isValid2 = true;

		if (name.split(" ").length == 1) {
			isValid1 = true;
		} else
			isValid1 = false;

		for (int i = 0; i < name.length(); i++) {
			if (name.charAt(i) < '0' || (name.charAt(i) > '9' && name.charAt(i) < 'a') || name.charAt(i) > 'z') {
				isValid2 = false;
			}
		}
		return isValid1 && isValid2;
	}

	static class ClientSender extends Thread {
		Socket socket;
		DataOutputStream out;
		String menu;
		String user;
		String author;
		String title;
		String searchString;
		String msg;

		ClientSender(Socket socket, String user) {
			this.socket = socket;
			try {
				out = new DataOutputStream(socket.getOutputStream());
				this.menu = "-";
				this.user = user;
				this.author = "-";
				this.title = "-";
				this.searchString = "-";
				this.msg = "-";
			} catch (Exception e) {
			}
		}

		public void run() {
			Scanner scanner = null;
			try {
				if(out!=null) {
					//System.out.println("sending msg to server at first : "+user);
					out.writeUTF(user); // at first, only username is sent
				}
				while (out != null) {
					while (true) {
						try {
						Thread.sleep(10);
						}catch(InterruptedException e) {
						}
						System.out.print(this.user + " >> ");
						//System.out.println("Input command");

						scanner = new Scanner(System.in);
						menu = scanner.nextLine();
						//System.out.println("your input : " + menu);
						menu=menu.trim();
						// in case of add
						if (menu.equalsIgnoreCase("add")) {
							System.out.print("add-title> ");
							scanner = new Scanner(System.in);
							title = scanner.nextLine();
							if (title.isEmpty()) {
								//System.out.println("wrong title input");
								continue;
							}
							System.out.print("add-author> ");
							scanner = new Scanner(System.in);
							author = scanner.nextLine();
							if (author.isEmpty()) {
								//System.out.println("wrong author input");
								continue;
							}
							// msg form : menu|user|title|author|searchString
							// menu,user okay, title/author inputted, searchstring don't need
							msg = menu + "\t" + user + "\t" + title + "\t" + author + "\t" + searchString;
							//System.out.println("sending msg to server at add : "+msg);
							out.writeUTF(msg);

						} else if (menu.equalsIgnoreCase("borrow")) {
							System.out.print("borrow-title> ");
							scanner = new Scanner(System.in);
							title = scanner.nextLine();
							if (title.isEmpty()) {
								//System.out.println("wrong borrow input");
								continue;
							}
							// menu okay, user okay, title inputted, author/search don't need
							msg = menu + "\t" + user + "\t" + title + "\t" + author + "\t" + searchString;
							//System.out.println("sending msg to server at borrow : "+msg);
		
							out.writeUTF(msg);
						} else if (menu.equalsIgnoreCase("return")) {

							System.out.print("return-title> ");
							scanner = new Scanner(System.in);
							title = scanner.nextLine();
							if (title.isEmpty()) {
								//System.out.println("wrong return input");
								continue;
							}
							// menu,user okay, title inputted, author/search don't need
							msg = menu + "\t" + user + "\t" + title + "\t" + author + "\t" + searchString;
							//System.out.println("sending msg to server at return : "+msg);
							
							out.writeUTF(msg);
						} else if (menu.equalsIgnoreCase("info")) {

							// menu, user okay. title/auth/ss don't need
							msg = menu + "\t" + user + "\t" + title + "\t" + author + "\t" + searchString;
							//System.out.println("sending servermsg at info : "+msg);
							
							out.writeUTF(msg);
						} else if (menu.equalsIgnoreCase("search")) {
							while (true) {
								System.out.print("search-string> "); // get input
								scanner = new Scanner(System.in);
								searchString = scanner.nextLine();

								if (searchString.isEmpty()) { // empty, break
									break;
								} 
								else if (searchString.length() < 3) {
									System.out.println("Search string must be longer than 2 characters");
									continue;
								}
								else {
									msg = menu + "\t" + user + "\t" + title + "\t" + author + "\t" + searchString;
									//System.out.println("sending msg to server at search : "+msg);
									
									out.writeUTF(msg);
									break;
								}
							}
						}
						else {
							System.out.println();
							System.out.println("[available commands]");
							System.out.println("add: add a new book to the list of books.");
							System.out.println("borrow: borrow a book from the library.");
							System.out.println("return: return a book to the library.");
							System.out.println("return: return a book to the library.");
							System.out.println("info: show list of books I am currently borrowing.");
							System.out.println("search: search for books.");
							System.out.println();
						}
					}
				}
			} catch (IOException e) {
			}
		}
	}

	static class ClientReceiver extends Thread {
		Socket socket;
		DataInputStream in;

		ClientReceiver(Socket socket) {
			this.socket = socket;
			try {
				in = new DataInputStream(socket.getInputStream());
			} catch (IOException e) {
			}
		}

		public void run() {
			while (in != null) {
				try {
					System.out.println(in.readUTF());
				} catch (IOException e) {
				}
			}
		}
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub

		if (args.length != 2) {
			System.out.println("Please give the IP address and port number as arguments");
			System.exit(0);
		}
		try {
			String serverIp = args[0];
			int portNumber = Integer.valueOf(args[1]);
			Socket socket= null;
			
			try{
				socket=new Socket(serverIp, portNumber);
			}
			catch(Exception e) {
				System.out.println("Connection establishment failed.");
				System.exit(0);
			}
			
			System.out.println("connected to server.");
			String name = "";
			Scanner scanner = null;

			while (true) {
				System.out.print("Enter userID >> ");
				scanner = new Scanner(System.in);
				name = scanner.nextLine();
				if (isValidName(name))
					break;
				else {
					System.out.println("UserID must be a single word with lowercase alphabets and numbers.");
				}
			}
			//System.out.println("Hello " + name + "!");
			Thread sender = new Thread(new ClientSender(socket, name));
			Thread receiver = new Thread(new ClientReceiver(socket));
			sender.start();
			receiver.start();
		} catch (Exception e) {
		}

	}
}

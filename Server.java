import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
class Book implements Comparable<Book>{ // 필요할까?
	// map with title, author. ArrayList with borrower?
	// map with (title, author) and (borrower?)
	// two map with (title, author) and (title, borrower)?
	private String title;
	private String author;
	private String user;

	Book() {
		this.title = "noTitle";
		this.author = "noAuthor";
		this.user = "-";
	}

	Book(String newTitle, String newAuthor) {
		this.title = newTitle;
		this.author = newAuthor;
		this.user = "-";
	}

	Book(String newTitle, String newAuthor, String newUser) {
		this.title = newTitle;
		this.author = newAuthor;
		this.user = newUser;
	}

	void setTitle(String newTitle) {
		this.title = newTitle;
	}

	void setAuthor(String newAuthor) {
		this.author = newAuthor;
	}

	void setUser(String newUser) {
		this.user = newUser;
	}

	String getTitle() {
		return this.title;
	}

	String getAuthor() {
		return this.author;
	}

	String getUser() {
		return this.user;
	}

	@Override
	public int compareTo(Book e) {
		if(this.title.toLowerCase().compareToIgnoreCase(e.getTitle())>0)
			return 1;
		else if (this.title.toLowerCase().compareToIgnoreCase(e.getTitle())<0)
			return -1;
		else {
			return 0;
		}
	}
}

public class Server {

	HashMap<String,DataOutputStream> clients;
	static ArrayList<Book>BookList=new ArrayList<Book>();
	Server(){
		clients=new HashMap<>();
		Collections.synchronizedMap(clients);
		
	}
	

public static void readBooks(String fileInput) {
		//System.out.println("readBooks start");
		BufferedReader br;
		try {
			br = new BufferedReader(new FileReader(fileInput));
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			System.out.println("Input file not found");
			return;
		}
		while (true) {
			try {
				String line = br.readLine();
				if (line == null) {
					br.close();
					break;
				}

				String[] arr = line.split("\t");
				String tmpTitle = arr[0];
				String tmpAuthor = arr[1];
				String tmpUser = arr[2];
				Book tmpBook = new Book(tmpTitle, tmpAuthor, tmpUser);
				BookList.add(tmpBook);
				Collections.sort(BookList);
				writeOutputFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
		}
		return;
	}
	
	public static void writeOutputFile() {
		int totalSize = BookList.size();
		FileWriter fw;
		try {
			fw = new FileWriter("books.txt");
			for (int i = 0; i < BookList.size(); i++) {
				String str = "";
				str = BookList.get(i).getTitle() + "\t" + BookList.get(i).getAuthor() + "\t" + BookList.get(i).getUser()
						+ "\n";
				fw.write(str);
			}
			fw.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	

	
	public static boolean isDuplicate(String bookTitle) {
		// need to redesign, since booktitle is duplicate with case-insensitivity
		
		boolean result = false;
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getTitle().equalsIgnoreCase(bookTitle))
				result=true;
		}
		return result;
	}
	
	public static String isBorrowable(String bookTitle) {
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getTitle().equalsIgnoreCase(bookTitle)){
				if(BookList.get(i).getUser().equalsIgnoreCase("-"))
					return BookList.get(i).getTitle();
			}
		}
		return null;
	}
	
	public static void borrowBook(String bookTitle,String username) {
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getTitle().equalsIgnoreCase(bookTitle)){
				BookList.get(i).setUser(username);
				return;
			}
		}
	}
	
	public static String isReturnable(String bookTitle, String username) {
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getTitle().equalsIgnoreCase(bookTitle)){
				if(BookList.get(i).getUser().equals(username))
					return BookList.get(i).getTitle();
			}
		}
		return null;
	}
	
	public static void returnBook(String bookTitle) {
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getTitle().equalsIgnoreCase(bookTitle)) {
				BookList.get(i).setUser("-");
				return;
			}
		}
	}
	
	public static String showInfo(String username) {
		String res="";
		int bookCount=0;
		ArrayList<String> bookAuthInfo=new ArrayList<String>();
		
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getUser().equalsIgnoreCase(username)) {
				bookCount++;
				bookAuthInfo.add(BookList.get(i).getTitle()+", "+BookList.get(i).getAuthor()+"\n");
			}
		}
		res="You are currently borrowing "+bookCount+" books:"+"\n";
		for(int i=0;i<bookAuthInfo.size();i++) {
			res+=(i+1)+". "+bookAuthInfo.get(i);
		}
		return res;
	}
	
	public static String searchBook(String searchString) {
		String res="";
		int searchCount=0;
		ArrayList<String> searchInfo=new ArrayList<String>();
		
		for(int i=0;i<BookList.size();i++) {
			if(BookList.get(i).getTitle().toLowerCase().contains(searchString.toLowerCase())
					|| BookList.get(i).getAuthor().toLowerCase().contains(searchString.toLowerCase())){
				searchCount++;
				searchInfo.add(BookList.get(i).getTitle()+", "+BookList.get(i).getAuthor()+"\n");
			}
		}
		res="Your search matched "+searchCount+" results."+"\n";
		for(int i=0;i<searchInfo.size();i++) {
			res+=(i+1)+". "+searchInfo.get(i);
		}
		return res;
	}
	
	
	
	public void start(int portNumber) {
		ServerSocket serverSocket=null;
		Socket socket=null;
		try {
			serverSocket=new ServerSocket(portNumber);
			System.out.println("server has started");
			while(true) {
				socket=serverSocket.accept();
				//System.out.println("a new connection from ["+socket.getInetAddress()+":"+socket.getPort()+"]");
				ServerReceiver thread=new ServerReceiver(socket);
				thread.start();
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	void sendToAll(String msg) {
		Iterator<String> it=clients.keySet().iterator();
		while(it.hasNext()) {
			try {
				DataOutputStream out=(DataOutputStream)clients.get(it.next());
				out.writeUTF(msg);
			}
			catch(IOException e) {}
		}
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		if (args.length != 1) {
			System.out.println("Please give us the port number as an argument");
			System.exit(0);
		}
		try {
			readBooks("books.txt"); // read book from data, at first
			new Server().start(Integer.valueOf(args[0]));
		} catch (Exception e) {
		}
	}

	class ServerReceiver extends Thread{
		Socket socket;
		DataInputStream in;
		DataOutputStream out;
		
		ServerReceiver(Socket socket){
			this.socket=socket;
			try {
				in=new DataInputStream(socket.getInputStream());
				out=new DataOutputStream(socket.getOutputStream());
			}catch(IOException e) {}
		}
		
		public void run() {
			String inputMsg="";
			String outputMsg="";
			String menu=""; String title=""; String author=""; String user=""; String searchString="";
			try {
				
				inputMsg=in.readUTF();
				//sendToAll("#"+inputMsg+" has joined");
				//System.out.println("first msg from Client : "+inputMsg);
				out.writeUTF("Hello "+inputMsg+"!");
				clients.put(inputMsg, out);
				//System.out.println("current number of users: "+ clients.size());
				
				while(in!=null) {
					// msg form : menu|user|title|author|searchString
					inputMsg=in.readUTF();
					//System.out.println("Current msg from Client : "+inputMsg);
					String[] arr=inputMsg.split("\t");
					menu=arr[0]; user=arr[1]; title=arr[2]; author=arr[3]; searchString=arr[4];
					//System.out.println("menu : "+menu+", user : "+user+", title : "+title+", author : "+author+", searchString: "+searchString);
					if(menu.equalsIgnoreCase("add")) {
						if(isDuplicate(title)) {
							outputMsg="The book already exists in the list.";
							out.writeUTF(outputMsg);
						}
						else {
							// create new book
							Book tmpBook=new Book(title,author,"-");
							
							// add to bookData
							BookList.add(tmpBook);
							
							// sort
							Collections.sort(BookList);
							
							// write output.txt
							writeOutputFile();
							
							outputMsg="A new book added to the list.";
							out.writeUTF(outputMsg);
						}
					}
					else if(menu.equalsIgnoreCase("borrow")) {
						String res=isBorrowable(title);
						if(res==null) {
							outputMsg="The book is not available";
							out.writeUTF(outputMsg);
						}
						else {
							// do borrow activity
							borrowBook(title,user);
							Collections.sort(BookList);
							writeOutputFile();
							outputMsg="You borrowed a book. - "+res;
							out.writeUTF(outputMsg);
						}
					} 
					else if (menu.equalsIgnoreCase("return")) {
						String res= isReturnable(title,user);
						if(res==null) {
							outputMsg="You did not borrow the book";
							out.writeUTF(outputMsg);
						}
						else {
							// do return activity
							returnBook(title);
							Collections.sort(BookList);
							writeOutputFile();
							outputMsg="You returned a book. - "+res;
							out.writeUTF(outputMsg);
						}
					} 
					else if (menu.equalsIgnoreCase("info")) {
						String res=showInfo(user);
						if(res==null) {
							outputMsg="No information about user.";
							out.writeUTF(outputMsg);
						}
						else {
							outputMsg=res;
							out.writeUTF(outputMsg);
						}
					} 
					else if (menu.equalsIgnoreCase("search")) {
						String res=searchBook(searchString);
						if(res==null) {
							outputMsg="No search result";
							out.writeUTF(outputMsg);
						}
						else {
							outputMsg=res;
							out.writeUTF(outputMsg);	
						}
					}

				}
			}
			catch(IOException e) {
				
			}
			finally {
				//sendToAll("#"+user+" has left");
				clients.remove(user);
				//System.out.println("["+socket.getInetAddress()+":"+socket.getPort()+"] has disconnected");
				//System.out.println("current number of users: "+ clients.size());
			}
		}
	}
}

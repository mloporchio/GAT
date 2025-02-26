import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * Questa classe costruisce il multigrafo globale a partire dai trasferimenti ERC-20 di un contratto.
 * Il multigrafo globale è la rete di tutti i trasferimenti prodotti da un contratto.
 * - Ogni nodo rappresenta un indirizzo.
 * - Ogni arco orientato (u, v) rappresenta un trasferimento da u a v e viene etichettato con:
 * a)
 * b)
 * 
 * INPUT:
 * Un file CSV dove ciascuna riga contiene, nell'ordine, i seguenti 5 campi.
 * 
 * 1) Block identifier in which the transfer occurred.
 * 2) Numeric identifier of the contract that produced the event.
 * 3) Numeric identifier of the sender of the transfer.
 * 4) Numeric identifier of the recipient of the transfer.
 * 5) Amount of tokens transferred.
 * 
 * OUTPUT:
 * La classe produce in output i seguenti file.
 * 
 * 1)
 * 2)
 * 3)
 * 
 * PRINT:
 * Il programma stampa in output il numero di nodi, archi e il tempo impiegato per la costruzione.
 * 
 * @author Matteo Loporchio
 */
public class MultigraphBuilder {
	public static int nextId = 0;
	public static Map<Integer, Integer> nodes = new LinkedHashMap<>();
	public static List<MultigraphEdge> edges = new ArrayList<>();

    public static void main(String[] args) {
		final String inputFile = args[0];
		final String edgeListFile = args[1];
		final String nodeMapFile = args[2];
        
		long start = System.nanoTime();
        try (
				BufferedReader in = new BufferedReader(new InputStreamReader(new FileInputStream(inputFile)));
				PrintWriter edgeOut = new PrintWriter(edgeListFile);
				PrintWriter nodeOut = new PrintWriter(nodeMapFile);
		) {
			String line = null;
			while ((line = in.readLine()) != null) {
				String[] parts = line.split(",");
				// The line includes the following fields.
				// * 0) Block identifier in which the transfer occurred.
				// * 1) Numeric identifier of the contract that produced the event.
				// * 2) Numeric identifier of the sender of the transfer.
				// * 3) Numeric identifier of the recipient of the transfer.
				// * 4) Amount of tokens transferred.
				int fromAddress = Integer.parseInt(parts[2]);
				int toAddress = Integer.parseInt(parts[3]);
				// Transfers with sender = 0x0 or receiver = 0x0 are ignored.
				if (fromAddress != 0 && toAddress != 0)  {
					int fromId = getOrCreateId(fromAddress);
					int toId = getOrCreateId(toAddress);
                    double value = Double.parseDouble(parts[4]);
					edges.add(new MultigraphEdge(fromId, toId, value));
				}
			}
			// Sort the list of edges according to the (fromId, toId) fields.
			Collections.sort(edges);
			// Write the list of edges to the output file.
			for (MultigraphEdge e : edges) 
				edgeOut.printf("%d\t%d\t%f\n", e.fromId, e.toId, e.value);
			// Write the (address, id) associations to the corresponding file.
			for (int key : nodes.keySet())
				nodeOut.printf("%d\t%d\n", key, nodes.get(key));
			// Stampa in output il numero di nodi, archi e tempo impiegato per la costruzione.
			long end = System.nanoTime();
			System.out.printf("%d\t%d\t%d\n", nodes.size(), edges.size(), end-start);
		}
		catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
    }

	/**
	 * Assigns a unique and progressive numeric identifier to an address.
	 * @param address the input address
	 * @return an identifier for the current address
	 */
	public static int getOrCreateId(int address) {
		Integer id = nodes.get(address);
		if (id == null) {
			id = nextId;
			nodes.put(address, id);
			nextId++;
		}
		return id;
	}

}
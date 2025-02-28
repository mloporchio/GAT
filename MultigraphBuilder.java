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
 * This class reads the ERC-20 transfer list of a given contract and builds the corresponding multigraph.
 * In the multigraph, each node represents an Ethereum address, while each directed edge (u, v) represents 
 * a token transfer from address u to v and is labeled with the amount of tokens transferred.
 * 
 * INPUT:
 * The ERC-20 transfer list of a contract, i.e., a CSV file where each row includes the following fields:
 * 	1) block identifier in which the transfer occurred;
 * 	2) numeric identifier of the contract that produced the event;
 * 	3) numeric identifier of the sender of the transfer;
 * 	4) numeric identifier of the recipient of the transfer;
 * 	5) amount of tokens transferred.
 * 
 * OUTPUT:
 * The program outputs the following TSV files:
 * 	1) 	the weighted edge list of the multigraph, where each row includes the following fields:
 * 			- the numeric identifier of the sender;
 * 			- the numeric identifier of the recipient;
 * 			- the amount of tokens transferred;
 * 	2)	a TSV file containing the mapping between original address identifiers (i.e., those used
 * 		in the input ERC-20 transfer list) and numeric identifiers used in the edge list.
 * 
 * PRINT:
 * The program prints to stdout the number of nodes, edges, and the elapsed time for the construction
 * (all separated by a tab character).
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
				// Self-transfers are also ignored.
				if (fromAddress != 0 && toAddress != 0 && fromAddress != toAddress) {
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
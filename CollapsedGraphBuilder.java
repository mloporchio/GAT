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
 * This class constructs a collapsed graph from the ERC-20 transfer list of a contract.
 * 
 * In the collapsed graph:
 * - each node represents an Ethereum address;
 * - each directed edge (u, v) represents a transfer from address u to v and is labeled with:
 * 	- the total number of transfers from u to v;
 * 	- the total amount of tokens transferred from u to v.
 * 
 * Mint and burn transfers, as well as self-transfers, are ignored during graph construction.
 * 
 * INPUT:
 * The ERC-20 transfer list of a contract, i.e., a CSV file where each row includes the following fields:
 * 1) block identifier in which the transfer occurred;
 * 2) numeric identifier of the contract that produced the event;
 * 3) numeric identifier of the sender of the transfer;
 * 4) numeric identifier of the recipient of the transfer;
 * 5) amount of tokens transferred;
 * 
 * OUTPUT:
 * The program outputs the following TSV files:
 * 	1) 	the weighted edge list of the collapsed graph, where each row includes the following fields:
 * 			- the numeric identifier of the sender;
 * 			- the numeric identifier of the recipient;
 * 			- the total number of transfers from sender to recipient;	
 * 			- the total amount of tokens transferred from sender to recipient;
 * 	2)	a TSV file containing the mapping between original address identifiers (i.e., those used
 * 		in the input ERC-20 transfer list) and numeric identifiers used in the edge list.
 * 
 * PRINT:
 * The following information is printed to the standard output (separated by a tab character):
 * 1) number of nodes in the graph;
 * 2) number of edges in the graph;
 * 3) elapsed time for construction.
 *  
 * @author Matteo Loporchio
 */
public class CollapsedGraphBuilder {
	public static int nextId = 0;
	public static Map<Integer, Integer> nodes = new LinkedHashMap<>();
	public static List<MultigraphEdge> edges = new ArrayList<>();
	
	public static void main(String[] args) {
		if (args.length < 3) {
			System.err.printf("Usage: %s <inputFile> <edgeListFile> <nodeMapFile>\n", CollapsedGraphBuilder.class.getName());
			System.exit(1);
		}

		// Read all input parameters.
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
				// Transfers with sender = 0x0 (mint) or receiver = 0x0 (burn) are ignored.
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
			int numUniqueEdges = 0;
			if (edges.size() > 0) {
				MultigraphEdge prev = edges.get(0);
				int count = 1;
				double totalValue = prev.value;
				for (int i = 1; i < edges.size(); i++) {
					MultigraphEdge curr = edges.get(i);
					if (curr.equals(prev)) {
						count++;
						totalValue += curr.value;
					} 
					else {
						edgeOut.printf("%d\t%d\t%d\t%f\n", prev.fromId, prev.toId, count, totalValue);
						numUniqueEdges++;
						prev = curr;
						count = 1;
						totalValue = curr.value;
					}
				}
				edgeOut.printf("%d\t%d\t%d\t%f\n", prev.fromId, prev.toId, count, totalValue);
				numUniqueEdges++;
			}
			// Write the (address, id) associations to the corresponding file.
			for (int key : nodes.keySet())
				nodeOut.printf("%d\t%d\n", key, nodes.get(key));
			// Stampa in output il numero di nodi, archi e tempo impiegato per la costruzione.
			long end = System.nanoTime();
			System.out.printf("%d\t%d\t%d\n", nodes.size(), numUniqueEdges, end-start);
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
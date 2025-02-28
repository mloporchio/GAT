import it.unimi.dsi.webgraph.BVGraph;

/**
 * Transforms a textual (unweighted) edge list into a directed graph in the BVGraph format.
 * The BVGraph format is used by the WebGraph library 
 * (see https://webgraph.di.unimi.it/docs/it/unimi/dsi/webgraph/BVGraph.html).
 * The input edge list should be a text file in TSV format
 * (see https://webgraph.di.unimi.it/docs/it/unimi/dsi/webgraph/ArcListASCIIGraph.html).
 * 
 * INPUT:
 * - A TSV file representing the (unweighted) edge list of the graph.
 * 
 * OUTPUT:
 * - A set of files representing the directed graph in BVGraph format.
 * 
 * @author Matteo Loporchio
 */
public class WebGraphBuilder {
	public static void main(String[] args) {
		if (args.length < 2) {
			System.err.printf("Usage: java %s <inputFile> <outputPrefix>\n", WebGraphBuilder.class.getName());
			System.exit(1);
		}
		final String inputFile = args[0];
		final String outputPrefix = args[1];
		try {
			BVGraph.main(("-g ArcListASCIIGraph " + inputFile + " " + outputPrefix).split(" "));
		}
		catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}
}

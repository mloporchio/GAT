import it.unimi.dsi.webgraph.ImmutableGraph;
import it.unimi.dsi.webgraph.Transform;
import it.unimi.dsi.webgraph.algo.ConnectedComponents;
import it.unimi.dsi.webgraph.algo.HyperBall;
import it.unimi.dsi.webgraph.algo.NeighbourhoodFunction;

import com.martiansoftware.jsap.JSAP;
import com.martiansoftware.jsap.JSAPResult;
import com.martiansoftware.jsap.Parameter;
import com.martiansoftware.jsap.SimpleJSAP;
import com.martiansoftware.jsap.Switch;
import com.martiansoftware.jsap.UnflaggedOption;


/**
 * Computes the average shortest path length of a directed graph stored in the BVGraph format.
 * The BVGraph format is used by the WebGraph library 
 * (see https://webgraph.di.unimi.it/docs/it/unimi/dsi/webgraph/BVGraph.html).
 * 
 * INPUT:
 * - Path of the files representing the directed graph in BVGraph format.
 * 
 * PRINT:
 * The following information is printed to stdout (separated by a tab character):
 * - number of graph nodes;
 * - number of graph edges;
 * - average shortest path length;
 * - time elapsed for computation (in nanoseconds).
 * 
 * Note that the number of nodes, edges and diameter printed to stdout
 * refer to the largest weakly connected component of the graph 
 * if the corresponding option is passed.
 * 
 * @author Matteo Loporchio
 */
public class WebGraphDistance {
	public static final int log2m = 7; // Logarithm of the number of registers per counter (used by HyperBall).
	public static void main(String[] args) throws Exception {
		SimpleJSAP jsap = new SimpleJSAP(
			WebGraphDiameter.class.getName(),
			"Computes the average shortest path length of a graph stored in the BVGraph format.",
			new Parameter[]{
				new UnflaggedOption("filename", JSAP.STRING_PARSER, true, 
				"Path of the files representing the graph in BVGraph format."),
				new Switch("undirected", 'u', "undirected", 
				"Specifies whether the graph should be treated as undirected."),
				new Switch("comp", 'c', "comp", 
				"Specifies whether the computation should be performed on the largest weakly connected component of the graph.")
			}
		);

		// Retrieve and use parsed arguments
		JSAPResult config = jsap.parse(args);
		if (jsap.messagePrinted()) {
			System.err.println();
			System.err.println("Usage: java " + WebGraphDiameter.class.getName());
			System.err.printf(jsap.getHelp());
			System.exit(1);
		}
		String filename = config.getString("filename");
		boolean undirected = config.getBoolean("undirected");
		boolean comp = config.getBoolean("comp");

		long start = System.nanoTime();

		// Load the graph.
		ImmutableGraph graph = ImmutableGraph.load(filename);

		// If necessary, transform the graph into its undirected version.
		if (undirected) {
			ImmutableGraph symmetricalGraph = Transform.symmetrize(graph);
			graph = symmetricalGraph;
		}

		// If necessary, compute the largest weakly connected component.
		if (comp) {
			ImmutableGraph largestComp = ConnectedComponents.getLargestComponent(graph, 0, null);
			graph = largestComp;
		}

		// Compute the number of nodes and edges.
		int numNodes = graph.numNodes();
		long numEdges = graph.numArcs();
	
		// Run HyperBall to estimate the average shortest path length.
		HyperBall hb = new HyperBall(graph, log2m, null);
		hb.run();
		hb.close();
		double[] nf = hb.neighbourhoodFunction.toDoubleArray();
		double distance = NeighbourhoodFunction.averageDistance(nf);

		long elapsed = System.nanoTime() - start;
		System.out.printf("%d\t%d\t%f\t%d\n", numNodes, numEdges, distance, elapsed);
	}
}


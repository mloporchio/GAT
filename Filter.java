import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.math.BigDecimal;

/**
 * This class reads the ERC-20 transfer dataset (see: https://zenodo.org/records/10644077)
 * and filters the transfers created by a target contract, creating a single file with 
 * all information about the target contract transfers.
 * 
 * INPUT:
 * 1) path of the ERC-20 transfer file;
 * 2) path of the ERC-20 transfer values file;
 * 3) target contract identifier;
 * 4) target contract decimals.
 *
 * OUTPUT:
 * The program creates a new CSV file with the filtered transfers.
 * Each line represents a token transfer and includes the following fields:
 * 1) block identifier in which the transfer occurred.
 * 2) numeric identifier of the contract that produced the event.
 * 3) numeric identifier of the sender of the transfer.
 * 4) numeric identifier of the recipient of the transfer.
 * 5) amount of tokens transferred.
 * 
 * @author Matteo Loporchio
 */
public class Filter {
	public static void main(String[] args) {
        if (args.length < 5) {
			System.err.printf("Usage: %s <transferFile> <valuesFile> <targetContractId> <decimals> <outputFile>\n", Filter.class.getName());
			System.exit(1);
		}
        
		final String transferFile = args[0];
		final String valuesFile = args[1];
		final int targetContractId = Integer.parseInt(args[2]);
        final BigDecimal scaleFactor = BigDecimal.TEN.pow(Integer.parseInt(args[3]));
		final String outputFile = args[4];
        
		try (
			BufferedReader tin = new BufferedReader(new InputStreamReader(new FileInputStream(transferFile)));
			BufferedReader vin = new BufferedReader(new InputStreamReader(new FileInputStream(valuesFile)));
			PrintWriter out = new PrintWriter(outputFile);
		) {
			String line1 = null;
			String line2 = null;
			while (((line1 = tin.readLine()) != null) && ((line2 = vin.readLine()) != null)) {
				String[] parts = line1.split(",");
				int contractId = Integer.parseInt(parts[1]);
				if (contractId == targetContractId) {
                    double value = (new BigDecimal(line2)).divide(scaleFactor).doubleValue();
                    out.printf("%s,%d,%s,%s,%f\n", parts[0], contractId, parts[2], parts[3], value);
                }
			}
		}
		catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}
}
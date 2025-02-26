import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.math.BigDecimal;

/**
 * Questa classe legge il dataset dei trasferimenti ERC-20 
 * (nel formato originale descritto qui: https://zenodo.org/records/10644077) 
 * e crea un unico file contenente le informazioni relative ai trasferimenti del contratto di interesse.
 * 
 * INPUT:
 * 1) percorso del file dei trasferimenti;
 * 2) percorso del file con gli importi dei trasferimenti;
 * 3) identificativo del contratto di interesse;
 * 4) numero di decimali del contratto.
 *
 * OUTPUT:
 * un file CSV dove ciascuna riga contiene, nell'ordine, i seguenti 5 campi.
 * 
 * 1) Block identifier in which the transfer occurred.
 * 2) Numeric identifier of the contract that produced the event.
 * 3) Numeric identifier of the sender of the transfer.
 * 4) Numeric identifier of the recipient of the transfer.
 * 5) Amount of tokens transferred.
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
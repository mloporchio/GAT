import java.util.Objects;

/**
 * Classe che rappresenta un arco orientato del multigrafo globale.
 *
 * 
 * @author Matteo Loporchio
 */
public class MultigraphEdge implements Comparable<MultigraphEdge> {
	public final int fromId;
	public final int toId;
	public final double value;

	public MultigraphEdge(int fromId, int toId, double value) {
		this.fromId = fromId;
		this.toId = toId;
		this.value = value;
	}

    @Override
    public int hashCode() {
        return Objects.hash(this.fromId, this.toId);
    }
    
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof MultigraphEdge)) return false;
        if (o == this) return true;
        MultigraphEdge e = (MultigraphEdge) o;
        return (this.fromId == e.fromId && this.toId == e.toId);
    }

	@Override
	public int compareTo(MultigraphEdge o) {
		int s = Integer.compare(this.fromId, o.fromId);
		return ((s != 0) ? s : Integer.compare(this.toId, o.toId));
	}
}
package constructor;

public class IdTransition {
    //Attributes
    private Integer from;
    private Integer to;

    //Constructor
    public IdTransition () {
        from = null;
        to = null;
    }

    //Methods
    public Integer getFrom() {
        return from;
    }

    public void setFrom(Integer from) {
        this.from = from;
    }

    public Integer getTo() {
        return to;
    }

    public void setTo(Integer to) {
        this.to = to;
    }

    public IdTransition getClone () {
        IdTransition clone = new IdTransition();
        clone.setFrom(this.from);
        clone.setTo(this.to);
        return clone;
    }
}

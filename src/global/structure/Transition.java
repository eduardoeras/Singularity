package global.structure;

public class Transition {
    //Attributes
    private Instruction from;
    private Instruction to;
    private Event event;

    //Constructor

    //Methods
    public void setFrom (Instruction from) {
        this.from = from;
    }

    public Instruction getFrom () {
        return from;
    }

    public void setTo (Instruction to) {
        this.to = to;
    }

    public Instruction getTo () {
        return to;
    }

    public void setEvent (Event event) {
        this.event = event;
    }

    public Event getEvent () {
        return event;
    }
}

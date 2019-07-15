package global.structure;

public class State implements Instruction {
    //Attributes
    private Label label;
    private Id id;
    private Type type;

    //Constructor
    public State () {
        label = new Label();
        id = new Id();
    }

    //Methods
}

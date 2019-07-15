package global.structure;

public class State implements Instruction {
    //Attributes
    private Label label;
    private String id;
    private Type type;

    //Constructor
    public State () {
        label = new Label();
        id = "";
    }

    //Methods
    public void setId () {

    }

    public String getId () {
        return id;
    }
}

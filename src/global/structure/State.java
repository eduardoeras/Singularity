package global.structure;

import global.tools.IdGenerator;

public class State implements Instruction {
    //Attributes
    private String label;
    private Type type;
    private int id;

    //Constructor
    public State () {
        label = "";
        id = IdGenerator.getIntegerId();
    }

    //Methods
    public int getId () {
        return id;
    }

    public void setLabel (String label) {
        this.label = label;
    }

    public String getLabel () {
        return label;
    }

    public void setType (Type type) {
        this.type = type;
    }

    public Type getType () {
        return type;
    }
}

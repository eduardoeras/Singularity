package global.structure;
import global.tools.IdGenerator;

public class Level implements Instruction {
    //Attributes
    private String label;
    private Type type;
    private Visibility visibility;
    private int id;

    //Constructor
    public Level () {
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

    public void setVisibility (Visibility visibility) {
        this.visibility = visibility;
    }

    public Visibility getVisibility () {
        return visibility;
    }
}

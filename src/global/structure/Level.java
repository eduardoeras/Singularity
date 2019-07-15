package global.structure;

public class Level implements Instruction {
    //Attributes
    private Label label;
    private String id;
    private Type type;
    private Visibility visibility;

    //Constructor
    public Level () {
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

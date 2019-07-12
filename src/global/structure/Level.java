package global.structure;

public class Level implements Instruction {
    //Attributes
    private Label label;
    private Type type;
    private Visibility visibility;
    private Id id;

    //Constructor
    public Level () {
        label = new Label();
        id = new Id();
    }

    //Methods
}

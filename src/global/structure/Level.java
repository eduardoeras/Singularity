package global.structure;

public class Level implements Instruction {
    //Attributes
    private Label label;
    private Id id;
    private Type type;
    private Visibility visibility;

    //Constructor
    public Level () {
        label = new Label();
        id = new Id();
    }

    //Methods
}

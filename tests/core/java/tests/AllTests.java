package tests;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({
    tests.unit.AllUnitTests.class,
    tests.endtoend.AllEnd2EndTests.class
})
public class AllTests {

}

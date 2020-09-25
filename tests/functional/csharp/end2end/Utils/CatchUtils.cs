using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    using static Test.Diagnostics;

    public static class SPXTEST
    {
        public static void SPXTEST_REQUIRE(bool isTrue, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_REQUIRE(isTrue, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_REQUIRE(bool isTrue, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_REQUIRE: {message}" : $"SPXTEST_REQUIRE:";

            if (isTrue) SPX_TRACE_INFO("SPXTEST_REQUIRE: PASSED:", line, caller, file);
            if (!isTrue) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (!isTrue) Test.Diagnostics.DumpMemoryLog();

            Assert.IsTrue(isTrue, $"{message} at {caller} in {file}({line}): FAILED:");
        }


        public static void SPXTEST_ISTRUE(bool isTrue, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_ISTRUE(isTrue, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_ISTRUE(bool isTrue, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_ISTRUE: {message}" : $"SPXTEST_ISTRUE:";

            if (isTrue) SPX_TRACE_INFO("SPXTEST_ISTRUE: PASSED:", line, caller, file);
            if (!isTrue) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (!isTrue) Test.Diagnostics.DumpMemoryLog();

            Assert.IsTrue(isTrue, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static void SPXTEST_ISFALSE(bool condition, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_ISFALSE(condition, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_ISFALSE(bool condition, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_ISFALSE: {message}" : $"SPXTEST_ISFALSE:";

            if (!condition) SPX_TRACE_INFO("SPXTEST_ISFALSE: PASSED:", line, caller, file);
            if (condition) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (condition) Test.Diagnostics.DumpMemoryLog();

            Assert.IsFalse(condition, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static void SPXTEST_ISNOTNULL(object value, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_ISNOTNULL(value, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_ISNOTNULL(object value, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_ISNOTNULL: {message}" : $"SPXTEST_ISNOTNULL:";

            var notNull = (value != null);

            if (notNull) SPX_TRACE_INFO("SPXTEST_ISNOTNULL: PASSED:", line, caller, file);
            if (!notNull) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (!notNull) Test.Diagnostics.DumpMemoryLog();

            Assert.IsNotNull(value, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static void SPXTEST_ARE_EQUAL<T>(T t1, T t2, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_ARE_EQUAL(t1, t2, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_ARE_EQUAL<T>(T t1, T t2, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_ARE_EQUAL: {message}" : $"SPXTEST_ARE_EQUAL:";

            var c1 = t1 as ICollection;
            var c2 = t2 as ICollection;
            var areCollections = c1 != null && c2 != null;

            var isEqual = areCollections ? AreCollectionsEqual(c1, c2) : Object.Equals(t1, t2);

            if (isEqual) SPX_TRACE_INFO("SPXTEST_ARE_EQUAL: PASSED:", line, caller, file);
            if (!isEqual) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (!isEqual) Test.Diagnostics.DumpMemoryLog();

            if (areCollections) CollectionAssert.AreEqual(t1 as ICollection, t2 as ICollection, $"{message} at {caller} in {file}({line}): FAILED:");
            if (!areCollections) Assert.AreEqual(t1, t2, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static void SPXTEST_ARE_NOT_EQUAL<T>(T t1, T t2, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_ARE_NOT_EQUAL(t1, t2, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_ARE_NOT_EQUAL<T>(T t1, T t2, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_ARE_NOT_EQUAL: {message}" : $"SPXTEST_ARE_NOT_EQUAL:";

            var c1 = t1 as ICollection;
            var c2 = t2 as ICollection;
            var areCollections = c1 != null && c2 != null;

            var isEqual = areCollections ? AreCollectionsEqual(c1, c2) : Object.Equals(t1, t2);

            if (!isEqual) SPX_TRACE_INFO("SPXTEST_ARE_NOT_EQUAL: PASSED:", line, caller, file);
            if (isEqual) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (isEqual) Test.Diagnostics.DumpMemoryLog();

            if (areCollections) CollectionAssert.AreNotEqual(t1 as ICollection, t2 as ICollection, $"{message} at {caller} in {file}({line}): FAILED:");
            if (!areCollections) Assert.AreNotEqual(t1, t2, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static void SPXTEST_FAIL(string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_FAIL(string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_FAIL(string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_FAIL: {message}" : $"SPXTEST_FAIL:";

            SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            Test.Diagnostics.DumpMemoryLog();

            Assert.Fail($"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static T SPXTEST_THROWS<T>(Action action, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null) where T : Exception
        {
            return SPXTEST_THROWS<T>(action, string.Format(message, args), line, caller, file);
        }

        public static T SPXTEST_THROWS<T>(Action action, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null) where T : Exception
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_THROWS: {message}" : $"SPXTEST_THROWS:";

            try
            {
                action();
            }
            catch (T ex)
            {
                SPX_TRACE_INFO("SPXTEST_THROWS: PASSED:", line, caller, file);
                return ex;
            }
            catch
            {
                // do nothing
            }

            SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            Test.Diagnostics.DumpMemoryLog();

            Assert.ThrowsException<T>(() => null, $"{message} at {caller} in {file}({line}): FAILED:");
            return null;
        }

        public static void SPXTEST_THROWS<T>(Func<object> action, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null) where T : Exception
        {
            SPXTEST_THROWS<T>(action, string.Format(message, args), line, caller, file);
        }

        public static T SPXTEST_THROWS<T>(Func<object> action, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null) where T : Exception
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_THROWS: {message}" : $"SPXTEST_THROWS:";

            try
            {
                action();
            }
            catch (T ex)
            {
                SPX_TRACE_INFO("SPXTEST_THROWS: PASSED:", line, caller, file);
                return ex;
            }
            catch
            {
                // do nothing
            }

            SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            Test.Diagnostics.DumpMemoryLog();

            Assert.ThrowsException<T>(() => null, $"{message} at {caller} in {file}({line}): FAILED:");
            return null;
        }

        public static void SPXTEST_NOTHROW(Action action, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_NOTHROW(action, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_NOTHROW(Action action, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_NOTHROW: {message}" : $"SPXTEST_NOTHROW:";

            Exception exception = null;
            try
            {
                action();
            }
            catch (Exception ex)
            {
                exception = ex;
            }

            if (exception == null) SPX_TRACE_INFO("SPXTEST_NOTHROW: PASSED:", line, caller, file);
            if (exception != null) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (exception != null) Test.Diagnostics.DumpMemoryLog();

            Assert.IsTrue(exception == null, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        public static void SPXTEST_NOTHROW(Func<object> action, string message, object[] args, [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            SPXTEST_NOTHROW(action, string.Format(message, args), line, caller, file);
        }

        public static void SPXTEST_NOTHROW(Func<object> action, string message = "", [CallerLineNumber]int line = 0, [CallerMemberName]string caller = null, [CallerFilePath]string file = null)
        {
            message = !string.IsNullOrEmpty(message) ? $"SPXTEST_NOTHROW: {message}" : $"SPXTEST_NOTHROW:";

            Exception exception = null;
            try
            {
                action();
            }
            catch (Exception ex)
            {
                exception = ex;
            }

            if (exception == null) SPX_TRACE_INFO("SPXTEST_NOTHROW: PASSED:", line, caller, file);
            if (exception != null) SPX_TRACE_ERROR($"{message} FAILED:", line, caller, file);
            if (exception != null) Test.Diagnostics.DumpMemoryLog();

            Assert.IsTrue(exception == null, $"{message} at {caller} in {file}({line}): FAILED:");
        }

        private static bool AreCollectionsEqual(ICollection t1, ICollection t2)
        {
            if (t1.Count != t2.Count) return false;

            var iter1 = t1.GetEnumerator();
            var iter2 = t2.GetEnumerator();
            if (iter1 == null || iter2 == null) return false;

            while (iter1.MoveNext() && iter2.MoveNext())
            {
                var obj1 = iter1.Current;
                var obj2 = iter2.Current;
                var isEqual = (obj1 == null && obj2 == null) || (obj1 != null && obj2 != null && obj1.Equals(obj2));
                if (!isEqual) return false;
            }

            return true;
        }
    }

    /// <summary>
    /// Helper class to make it easier to reuse code from C++. Include the following using in your file:
    /// using static Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils.CatchUtils;
    /// </summary>
    public static class CatchUtils
    {
        public static void REQUIRE_NOTHROW(Action action, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            try
            {
                action();
            }
            catch (Exception e)
            {
                Assert.Fail(
                    "[{0}:{1}] {2} exception thrown where none was expected. {3}",
                    caller,
                    line,
                    e.GetType().Name,
                    e.ToString());
            }
        }

        public static void REQUIRE_THAT(string text, Catch.IMatcher matcher, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            Assert.IsTrue(matcher.IsMatch(text), "[{0}:{1}] Expected '{2}'. Got '{3}'", caller, line, matcher.ToString(), text);
        }

        public static void REQUIRE_NOTHROW(Task t, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            Exception ex = null;
            try
            {
                t.Wait();
            }
            catch (AggregateException ae)
            {
                // unwrap exception
                ae = ae.Flatten();
                if (ae.InnerExceptions.Count == 1)
                {
                    ex = ae.InnerException;
                }
            }
            catch (Exception e)
            {
                ex = e;
            }

            if (ex != null)
            {
                Assert.Fail(
                    "[{0}:{1}] {2} exception thrown where none was expected. {3}",
                    caller,
                    line,
                    ex.GetType().Name,
                    ex.ToString());
            }
        }

        public static void REQUIRE_THROWS_WITH(Task t, Catch.IMatcher matcher, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            Exception ex = null;
            try
            {
                t.Wait();
            }
            catch (AggregateException ae)
            {
                // unwrap exception
                ae = ae.Flatten();
                if (ae.InnerExceptions.Count == 1)
                {
                    ex = ae.InnerException;
                }
            }
            catch (Exception e)
            {
                ex = e;
            }

            if (ex != null)
            {
                Assert.IsTrue(matcher.IsMatch(ex.Message),
                   "[{0}:{1}] Expected an exception with {2}. Got {3} {4}",
                   caller,
                   line,
                   matcher.ToString(),
                   ex.GetType().FullName,
                   ex);
            }
            else
            {
                Assert.Fail(
                    "[{0}:{1}] No exception was thrown! Expected an exception with {2}",
                    caller,
                    line,
                    matcher.ToString());
            }
        }

        public static void REQUIRE_THROWS_MATCHES<TException>(Task t, Catch.IMatcher matcher, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            REQUIRE_THROWS_MATCHES(t, typeof(TException), matcher, caller, line, file);
        }

        public static void REQUIRE_THROWS_MATCHES(Task t, Type exceptionType, Catch.IMatcher matcher, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            REQUIRE_THROWS_MATCHES(() => t.Wait(), exceptionType, matcher, caller, line, file);
        }

        public static void REQUIRE_THROWS_MATCHES(Action action, Type exceptionType, Catch.IMatcher matcher, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            Exception ex = null;
            try
            {
                action();
            }
            catch (AggregateException ae)
            {
                // unwrap exception
                ae = ae.Flatten();
                if (ae.InnerExceptions.Count == 1)
                {
                    ex = ae.InnerException;
                }
            }
            catch (Exception e)
            {
                ex = e;
            }

            if (ex != null)
            {
                Assert.IsTrue(exceptionType.IsAssignableFrom(ex.GetType()) && matcher.IsMatch(ex),
                    "[{0}:{1}] Expected an {2} exception with {3}. Got {4} {5}",
                    caller,
                    line,
                    exceptionType.FullName,
                    matcher.ToString(),
                    ex.GetType().FullName,
                    ex);
            }
            else
            {
                Assert.Fail(
                    "[{0}:{1}] No exception was thrown! Expected a {2} exception with {3}",
                    caller,
                    line,
                    exceptionType.FullName,
                    matcher.ToString());
            }
        }
    }

    /// <summary>
    /// Simple implementation of the minimum from the Catch C++ library we need using C# Assert
    /// </summary>
    public class Catch
    {
        public enum CaseSensitive
        {
            Yes,
            No
        }

        public interface IMatcher
        {
            bool IsMatch(object other);
        }

        public abstract class MatcherBase : IMatcher
        {
            public abstract bool IsMatch(object other);

            public static MatcherBase operator &(MatcherBase a, MatcherBase b)
                => new LogicalMatcher(LogicalMatcher.Operation.And, a, b);

            public static MatcherBase operator |(MatcherBase a, MatcherBase b)
                => new LogicalMatcher(LogicalMatcher.Operation.Or, a, b);

            public static MatcherBase operator !(MatcherBase a)
                => new LogicalMatcher(LogicalMatcher.Operation.Not, a, null);
        }

        internal class LogicalMatcher : MatcherBase
        {
            private IMatcher _a;
            private IMatcher _b;
            private readonly Operation _op;

            public enum Operation
            {
                Not,
                And,
                Or
            }

            public LogicalMatcher(Operation op, IMatcher a, IMatcher b)
            {
                if (a == null)
                {
                    throw new ArgumentNullException(nameof(a));
                }
                else if (op > Operation.Not && b == null)
                {
                    throw new ArgumentNullException(nameof(b));
                }

                _op = op;
                _a = a;
                _b = b;
            }

            public override bool IsMatch(object other)
            {
                
                switch (_op)
                {
                    case Operation.Not:
                        return !_a.IsMatch(other);

                    case Operation.And:
                        return _a.IsMatch(other) && _b.IsMatch(other);

                    case Operation.Or:
                        return _a.IsMatch(other) || _b.IsMatch(other);

                    default:
                        return false;
                }
            }
        }

        internal class StringEqualityMatcher : MatcherBase
        {
            private string _v;
            private StringComparison _c;
            private string _op;

            public StringEqualityMatcher(string val, CaseSensitive cs)
                : this(val, cs, "==")
            {
            }

            protected StringEqualityMatcher(string val, CaseSensitive cs, string op)
            {
                _v = val;
                _c = cs == CaseSensitive.Yes
                    ? StringComparison.InvariantCulture
                    : StringComparison.InvariantCultureIgnoreCase;
                _op = op;
            }

            public override bool IsMatch(object other) => IsMatch(_v, other as string, _c);

            public override string ToString()
            {
                return $"<{_c.ToString()}> {_op} {_v}";
            }

            protected virtual bool IsMatch(string left, string right, StringComparison comparison)
            {
                return string.Equals(left, right, comparison);
            }
        }

        internal class StringContainsMatcher : StringEqualityMatcher
        {
            public StringContainsMatcher(string val, CaseSensitive cs)
                : base(val, cs, "contains")
            {
            }

            protected override bool IsMatch(string left, string right, StringComparison comparison)
            {
                return left != null
                    && right != null
                    && right.IndexOf(left, comparison) >= 0;
            }
        }

        internal class ExceptionMessageContainsMatcher : StringContainsMatcher
        {
            public ExceptionMessageContainsMatcher(string val, CaseSensitive cs)
                : base(val, cs)
            {
            }

            public override bool IsMatch(object other)
                => base.IsMatch((other as Exception)?.Message);

            public override string ToString()
                => "Exception with message " + base.ToString();
        }

        internal class FuzzyStringMatcher : MatcherBase
        {
            private string _expected;
            private int _deltaPercentage;
            private int _minMismatch;

            public FuzzyStringMatcher(string expected, int deltaPercentage, int minAllowedMismatchCount)
            {
                _expected = expected;
                _deltaPercentage = deltaPercentage;
                _minMismatch = minAllowedMismatchCount;
            }

            public override bool IsMatch(object other)
                => SpeechRecognitionTestsHelper.IsWithinStringWordEditPercentage(_expected, (other as string) ?? string.Empty, _deltaPercentage, _minMismatch);

            public override string ToString()
                => $"fuzzy match against '{_expected}' with {_deltaPercentage}% and {_minMismatch} minimum mismatches";
        }

        public static MatcherBase Equals(string str, CaseSensitive cs)
        {
            return new StringEqualityMatcher(str, cs);
        }

        public static MatcherBase Contains(string str, CaseSensitive cs)
        {
            return new StringContainsMatcher(str, cs);
        }

        public static MatcherBase FuzzyMatch(string str, int deltaPercentage = 10, int minAllowedMismatchCount = 1)
        {
            return new FuzzyStringMatcher(str, deltaPercentage, minAllowedMismatchCount);
        }

        public static Catch.MatcherBase HasHR(string hrString)
        {
            return new Catch.ExceptionMessageContainsMatcher(hrString, Catch.CaseSensitive.Yes);
        }

        public static Catch.MatcherBase ExceptionMessageContains(string message, Catch.CaseSensitive cs = Catch.CaseSensitive.Yes)
        {
            return new Catch.ExceptionMessageContainsMatcher(message, cs);
        }
    }
}

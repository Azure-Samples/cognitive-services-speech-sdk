using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    /// <summary>
    /// Helper class to make it easier to reuse code from C++. Include the following using in your file:
    /// using static Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils.CatchUtils;
    /// </summary>
    public static class CatchUtils
    {
        public static void REQUIRE(bool cond, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
        {
            Assert.IsTrue(cond, "[{0}:{1}] Condition is false", caller, line);
        }

        public static void SPXTEST_REQUIRE(bool cond, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0, [CallerFilePath]string file = null)
            => REQUIRE(cond, caller, line, file);

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

        public static void SPX_TRACE_INFO(string format, params object[] args)
        {
            string formatted = string.Format(
                CultureInfo.InvariantCulture,
                format,
                args);
            Console.WriteLine(formatted);

            // prefix with timestamp
            System.Diagnostics.Debug.WriteLine("({0}) {1}", DateTime.UtcNow.ToString("yyyy-MM-dd HH::mm::ss.ff"), formatted);
        }

        public static Catch.MatcherBase HasHR(string hrString)
        {
            return new Catch.ExceptionMessageContainsMatcher(hrString, Catch.CaseSensitive.Yes);
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

        public class LogicalMatcher : MatcherBase
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

        public class StringEqualityMatcher : MatcherBase
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

        public class StringContainsMatcher : StringEqualityMatcher
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

        public class ExceptionMessageContainsMatcher : StringContainsMatcher
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

        public static MatcherBase Equals(string str, CaseSensitive cs)
        {
            return new StringEqualityMatcher(str, cs);
        }

        public static MatcherBase Contains(string str, CaseSensitive cs)
        {
            return new StringContainsMatcher(str, cs);
        }
    }
}

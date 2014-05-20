//-*- coding: utf-8-unix -*-
/**    
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * see also http://brodowsky.it-sky.net/2013/08/09/closures-ii-java/ for explanation (in German)
 *
 * using inner classes to do Closures in Java 7 or older
 */
public interface Function<X, Y> {
    public Y f(X x);
}

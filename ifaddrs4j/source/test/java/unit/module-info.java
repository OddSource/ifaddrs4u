/*
 * Copyright © 2010-2023 OddSource Code (license@oddsource.io)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

@SuppressWarnings("requires-automatic")
module io.oddsource.java.test.unit
{
    exports io.oddsource.java.test.unit;
    requires java.base;
    requires io.oddsource.java.net.ifaddrs4j;
    requires junit;
    opens io.oddsource.java.test.unit to junit, org.apache.logging.log4j, org.apache.logging.log4j.core;
}

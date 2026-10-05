"use strict";

const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");
const { renderVehicleFixture } = require("../web/fixture/app.js");

const html = fs.readFileSync(path.join(__dirname, "../web/fixture/index.html"), "utf8");
const fixtureMatch = html.match(/<script id="fixture-data" type="application\/json">([\s\S]*?)<\/script>/);
assert.ok(fixtureMatch, "embedded fixture data must exist");
const fixtures = JSON.parse(fixtureMatch[1]);

const unresolved = renderVehicleFixture(fixtures.unresolved);
assert.equal(unresolved, renderVehicleFixture(fixtures.unresolved), "rendering is deterministic");
assert.match(unresolved, /manual_selection_required/);
assert.match(unresolved, /Capability registry unavailable/);
assert.match(unresolved, /No normalized samples/);
assert.doesNotMatch(unresolved, /Active profile ID/);

const selected = renderVehicleFixture(fixtures.selected);
assert.equal(selected, renderVehicleFixture(fixtures.selected), "selected fixture rendering is deterministic");
assert.match(selected, /Active profile ID: <code>1<\/code>/);
assert.match(html, /Selected Kamiq_NW4 fixture \(identity 1\)/);
const capabilityRows = selected.match(/<caption>Capabilities<\/caption>[\s\S]*?<tbody>([\s\S]*?)<\/tbody>/);
assert.ok(capabilityRows, "capability table must render");
assert.equal((capabilityRows[1].match(/<td>pending<\/td>/g) || []).length, 4);
assert.match(selected, /42\.5/);
assert.match(selected, /stale/);
assert.match(selected, /unavailable/);
assert.match(selected, /pending/);
assert.match(selected, /unknown/);
assert.match(selected, /9007199254740993/);
assert.match(selected, /Value not present/);
assert.match(selected, /<td>false<\/td>/, "an actual false value remains distinguishable from a missing value");
assert.match(html, /does not connect to a vehicle/);

const escaped = renderVehicleFixture({
  profileSelection: "<script>", capabilities: [], samples: [{
    signalId: "x<y", unit: "", source: "unknown", quality: "invalid_or_unknown",
    availability: "unknown", timestampMs: "0"
  }]
});
assert.doesNotMatch(escaped, /<script>/);
assert.match(escaped, /&lt;script&gt;/);
assert.match(escaped, /x&lt;y/);
const malformedBoolean = renderVehicleFixture({
  profileSelection: "unknown", capabilities: [], samples: [{
    signalId: "example.flag", unit: "", valueType: "boolean", value: "",
    source: "unknown", quality: "invalid_or_unknown", availability: "available",
    timestampMs: "1"
  }]
});
assert.match(malformedBoolean, /Invalid value omitted/);

console.log("Web fixture tests passed");

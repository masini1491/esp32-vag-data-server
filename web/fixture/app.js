(function (root) {
  "use strict";

  function escapeHtml(value) {
    return String(value).replace(/[&<>"']/g, function (character) {
      return { "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[character];
    });
  }

  function displayValue(sample) {
    if (!Object.prototype.hasOwnProperty.call(sample, "value")) {
      return "Value not present";
    }
    if (sample.valueType === "text" && typeof sample.value === "string") return escapeHtml(sample.value);
    if (sample.valueType === "boolean" && typeof sample.value === "boolean") return sample.value ? "true" : "false";
    if (sample.valueType === "numeric" && typeof sample.value === "number" && Number.isFinite(sample.value)) {
      return escapeHtml(sample.value);
    }
    return "Invalid value omitted";
  }

  function renderVehicleFixture(payload) {
    if (!payload || !Array.isArray(payload.samples) ||
        !(payload.capabilities === null || Array.isArray(payload.capabilities))) {
      throw new TypeError("Invalid vehicle data fixture");
    }

    var profile = "<p>Selection: <strong>" + escapeHtml(payload.profileSelection) + "</strong></p>";
    if (Object.prototype.hasOwnProperty.call(payload, "activeProfileId")) {
      profile += "<p>Active profile ID: <code>" + escapeHtml(payload.activeProfileId) + "</code></p>";
    }

    var capabilities;
    if (payload.capabilities === null) {
      capabilities = "<p>Capability registry unavailable; support is not inferred.</p>";
    } else if (payload.capabilities.length === 0) {
      capabilities = "<p>No capability descriptors.</p>";
    } else {
      capabilities = "<table><caption>Capabilities</caption><thead><tr><th>Signal</th><th>Type</th><th>Unit</th><th>Support</th></tr></thead><tbody>";
      payload.capabilities.forEach(function (capability) {
        capabilities += "<tr><td><code>" + escapeHtml(capability.signalId) + "</code></td><td>" +
          escapeHtml(capability.valueType) + "</td><td>" + escapeHtml(capability.unit) + "</td><td>" +
          escapeHtml(capability.support) + "</td></tr>";
      });
      capabilities += "</tbody></table>";
    }

    var samples;
    if (payload.samples.length === 0) {
      samples = "<p>No normalized samples.</p>";
    } else {
      samples = "<table><caption>Normalized samples</caption><thead><tr><th>Signal</th><th>Value</th><th>Unit</th><th>Source</th><th>Quality</th><th>Availability</th><th>Timestamp (ms)</th></tr></thead><tbody>";
      payload.samples.forEach(function (sample) {
        samples += "<tr><td><code>" + escapeHtml(sample.signalId) + "</code></td><td>" +
          displayValue(sample) + "</td><td>" + escapeHtml(sample.unit) + "</td><td>" +
          escapeHtml(sample.source) + "</td><td>" + escapeHtml(sample.quality) + "</td><td>" +
          escapeHtml(sample.availability) + "</td><td><code>" + escapeHtml(sample.timestampMs) + "</code></td></tr>";
      });
      samples += "</tbody></table>";
    }

    return "<section><h2>Profile</h2>" + profile + "</section><section>" + capabilities +
      "</section><section>" + samples + "</section>";
  }

  if (typeof module !== "undefined" && module.exports) {
    module.exports = { renderVehicleFixture: renderVehicleFixture };
  }

  if (root.document) {
    var fixtures = JSON.parse(root.document.getElementById("fixture-data").textContent);
    var selector = root.document.getElementById("fixture-select");
    var view = root.document.getElementById("fixture-view");
    function renderSelection() {
      view.innerHTML = renderVehicleFixture(fixtures[selector.value]);
    }
    selector.addEventListener("change", renderSelection);
    renderSelection();
  }
}(typeof globalThis !== "undefined" ? globalThis : this));
